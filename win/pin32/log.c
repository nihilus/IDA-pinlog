#include        "defs.h"


void    *log_init(){
        HANDLE  hSection;
        PVOID   lpBuffer;
        
        hSection = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, L"pin_map_file");
        //if (hSection == NULL) ExitProcess(0);
        lpBuffer = MapViewOfFile(hSection, FILE_MAP_READ | FILE_MAP_WRITE, 0,0, 0);
        return lpBuffer;
}


void    log_increment(volatile unsigned char *data){
        //_InterlockedIncrement(data);
        if (*data > 0xE0) return;
        __asm mov eax, data
        __asm lock inc byte ptr[eax]
        
}