#include        "defs.h"

LPCWSTR LOW_INTEGRITY_SDDL_SACL_W = L"S:(ML;;NW;;;LW)";
 
BOOL SetObjectToLowIntegrity(HANDLE hObject, SE_OBJECT_TYPE type){
        BOOL b_ret = FALSE;
        DWORD dwErr = ERROR_SUCCESS;
        PSECURITY_DESCRIPTOR pSD = NULL;
        PACL pSacl = NULL;
        BOOL fSaclPresent = FALSE;
        BOOL fSaclDefaulted = FALSE;
 
        if ( ConvertStringSecurityDescriptorToSecurityDescriptorW (LOW_INTEGRITY_SDDL_SACL_W, SDDL_REVISION_1, &pSD, NULL )){
                if ( GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted )){
                        dwErr = SetSecurityInfo(hObject, type, LABEL_SECURITY_INFORMATION,NULL, NULL, NULL, pSacl);
                        b_ret = (ERROR_SUCCESS == dwErr);
                }
                LocalFree ( pSD );
        }
 
        return b_ret;
}

int __cdecl wmain(int argc, wchar_t **argv){
        HANDLE  hSection, hFile;
        PVOID   lpMappedFile;
        WCHAR   *wsLogFileName;
        PROCESS_INFORMATION     pinfo;
        STARTUPINFO             sinfo;
        WCHAR   wsCommandLine[MAX_PATH * 10];
        WCHAR   wsPinToolPath[MAX_PATH + 2];
        WCHAR   *ws;
        DWORD   dwWritten;
        unsigned char *ptr;
        unsigned long index;
        unsigned char peheader[0x2000];
        PIMAGE_DOS_HEADER       pmz;
        PPEHEADER32             pe32;
        BOOL                    b_pin32;
        
        if (argc != 4){
                printf("Usage :\n");
                printf("        pinlog <fullpath> <module_to_trace> <logfile.log>\n");
                printf("example:\n");
                printf("        pinlog \"C:\\program files\\Internet Explorer\\iexplore.exe\" iexplore.exe log.log");
                return 0;
        }
                
        hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0,0);
        if (hFile == INVALID_HANDLE_VALUE){
                wprintf(L"[X] Can't open : %s\n", argv[1]);
                return 1;
        }
        
        if (!ReadFile(hFile, peheader, 0x2000, &dwWritten, 0)){
                wprintf(L"[X] Can't read from file : %s\n", argv[1]);
                CloseHandle(hFile);
                return 1;        
        }
        
        CloseHandle(hFile);
        
        pmz = (PIMAGE_DOS_HEADER)peheader;
        if (pmz->e_magic != IMAGE_DOS_SIGNATURE){
                wprintf(L"[X] File is not MZ file : %s\n", argv[1]);
                return 1;
        }
        
        if (pmz->e_lfanew > (0x2000 - 4 - sizeof(IMAGE_FILE_HEADER))){
                wprintf(L"e_lfanew is a little bit toooo big...\n");
                return 1;
        }
        
        pe32 = (PPEHEADER32)(peheader + pmz->e_lfanew);
        if (pe32->pe_signature != IMAGE_NT_SIGNATURE){
                wprintf(L"[X] file is not valid PE file : %s", argv[1]);
                return 1;
        }
        
        if (pe32->pe_magic == 0x20b)
                b_pin32 = FALSE;
        else
                b_pin32 = TRUE;
        

        memset(wsPinToolPath, 0, sizeof(wsPinToolPath));
        GetModuleFileName(GetModuleHandle(0), wsPinToolPath, sizeof(wsPinToolPath)/sizeof(WCHAR));
        ws = wcsrchr(wsPinToolPath, '\\');
        ws++;
        *ws = 0;
        if (b_pin32)
                StringCchCat(wsPinToolPath, sizeof(wsPinToolPath)/sizeof(WCHAR), L"pin32\\trace.dll");
        else
                StringCchCat(wsPinToolPath, sizeof(wsPinToolPath)/sizeof(WCHAR), L"pin64\\trace.dll");
                
        hSection = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x100000 * 50, L"pin_map_file");
        SetObjectToLowIntegrity(hSection, SE_KERNEL_OBJECT);
        
        memset(wsCommandLine, 0, sizeof(wsCommandLine));
        
        StringCchPrintf(wsCommandLine, sizeof(wsCommandLine)/sizeof(WCHAR),
                        L"pin -smc_strict 1 -follow_execv 1 -t \"%s\" -p %s -- \"%s\"",
                        wsPinToolPath,
                        argv[2],
                        argv[1]);
        
        memset(&sinfo, 0, sizeof(sinfo));
        memset(&pinfo, 0, sizeof(pinfo));
        CreateProcess(0,
                      wsCommandLine,
                      0,
                      0,
                      0,
                      0,
                      0,
                      0,
                      &sinfo,
                      &pinfo);
        WaitForSingleObject(pinfo.hProcess, INFINITE);
        
        lpMappedFile = MapViewOfFile(hSection, FILE_MAP_READ, 0,0,0);
        
        index = 0x100000 * 50 - 1;
        ptr = lpMappedFile;
        while (index != 0){
                if (ptr[index] != 0) break;
                index--;
        }
        if (index == 0){
                printf("No trace happened inside process\n");
                UnmapViewOfFile(lpMappedFile);
                CloseHandle(hSection);
                return 0;
        }
        index++;
        
        hFile = CreateFile(argv[3], GENERIC_WRITE, 0,0, CREATE_ALWAYS, 0,0);
        WriteFile(hFile, lpMappedFile, index, &dwWritten, 0);
        FlushFileBuffers(hFile);
        CloseHandle(hFile);
        UnmapViewOfFile(lpMappedFile);
        CloseHandle(hSection);
        
}
        
        