#include        "defspin.h"

PIN_LOCK        lock;

ADDRINT         process_start;
ADDRINT         process_end;
unsigned char   *pcounter;

KNOB<string> KnobProcessToTrace(KNOB_MODE_WRITEONCE, "pintool", "p", "", "No process selected to trace");

VOID  Image(IMG img, VOID *v){
        char    szFilePath[260];
        unsigned long index;
        
        GetLock(&lock, 1);
        if (process_start != 0){
                ReleaseLock(&lock);
                return;
        }
        if (IMG_Valid(img)){
                memset(szFilePath, 0, sizeof(szFilePath));
                strncpy(szFilePath, IMG_Name(img).c_str(), sizeof(szFilePath)-1);
                index = 0;
                while (szFilePath[index] != 0){
                        szFilePath[index] = tolower(szFilePath[index]);
                        index++;
                }
                
                if (strstr(szFilePath, KnobProcessToTrace.Value().c_str())){
                        process_start = IMG_LowAddress(img);
                        process_end   = IMG_HighAddress(img);
                }
        }
        ReleaseLock(&lock);
        
}

VOID    ImageUnload(IMG img, VOID *v){
        char    szFilePath[260];
        unsigned long index;
        
        GetLock(&lock, 1);
        if (IMG_Valid(img)){
                memset(szFilePath, 0, sizeof(szFilePath));
                strncpy(szFilePath, IMG_Name(img).c_str(), sizeof(szFilePath)-1);
                index = 0;
                while (szFilePath[index] != 0){
                        szFilePath[index] = tolower(szFilePath[index]);
                        index++;
                }
                
                if (strstr(szFilePath, KnobProcessToTrace.Value().c_str())){
                        process_start = 0;
                        process_end   = 0;
                }                       
        }
        ReleaseLock(&lock);
}


VOID    InstrumentFunction(char *insname, ADDRINT eip, THREADID tid, CONTEXT *pctx)
{
        unsigned long index;
        
        if (pcounter == NULL) return;
        index = eip - process_start;
        
        log_increment((volatile unsigned char *)&pcounter[index]);

} 

VOID    Trace(TRACE trace, VOID *v){
        ADDRINT addr;
        
        addr = TRACE_Address(trace);
        if (!(addr >= process_start & addr < process_end)) return;
        
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){
                for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins)){
                        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)InstrumentFunction, 
                                       IARG_PTR, 0, //new string(INS_Disassemble(ins)),
                                       IARG_INST_PTR,                                       
                                       IARG_THREAD_ID,
                                       IARG_CONTEXT,
                                       IARG_END);          
                }
        }
}

VOID  ContextChange(THREADID threadIndex, CONTEXT_CHANGE_REASON reason, const CONTEXT *from, CONTEXT *to, INT32 info, VOID *v){
        
        switch (reason)
        {
        case CONTEXT_CHANGE_REASON_APC:
                break;
        case CONTEXT_CHANGE_REASON_EXCEPTION: 
                break;
        case CONTEXT_CHANGE_REASON_CALLBACK:
                break;
        }
}

VOID ExitFunction(INT32 code, VOID *v){

}

int     __cdecl main(int argc, char *argv[]){        
        PIN_InitSymbols();

        if (PIN_Init(argc, argv)) return 1;
        InitLock(&lock);
        
        pcounter = (unsigned char *)log_init();

        TRACE_AddInstrumentFunction(Trace, 0);
        IMG_AddInstrumentFunction(Image, 0);
        IMG_AddUnloadFunction(ImageUnload, 0); 	
        PIN_AddContextChangeFunction(ContextChange, 0);
        PIN_AddFiniFunction(ExitFunction, 0);
        
        
        PIN_StartProgram();         
        return 0;
}
