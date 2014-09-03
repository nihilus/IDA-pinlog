#include        <stdio.h>
#include        <iostream>
#include	<string.h>
#include        "pin.H"

#include	<unistd.h>
#include	<sys/mman.h>
#include	<sys/stat.h>
#include	<fcntl.h>

KNOB<string> KnobProcessToTrace(KNOB_MODE_WRITEONCE, "pintool", "p", "", "process to trace");
    
#define SECTION_SIZE 0x100000 * 50
unsigned char *pcounter = (unsigned char *)MAP_FAILED;
unsigned long process_start;
unsigned long process_end;

VOID InstrumentFunc(char *s, THREADID tid, ADDRINT pc, CONTEXT *ctx){
	unsigned long index;
	if (pcounter == MAP_FAILED) return;
	index = pc - process_start;
	if (pcounter[index] > 0xE0) return;
	pcounter[index]++;	

}

VOID Trace(TRACE trace, VOID *v){
	ADDRINT addr;
	
	addr = TRACE_Address(trace);
		
	if (!(addr >= process_start && addr < process_end)) return;
	for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl)){                        
                for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins)){
			INS_InsertCall( ins,
					IPOINT_BEFORE,
					(AFUNPTR)InstrumentFunc,
					IARG_PTR, 0, //new string(INS_Disassemble(ins)),
					IARG_THREAD_ID,
					IARG_INST_PTR,
					IARG_CONTEXT,
					IARG_END);
		}
	}
}

VOID ImageLoad(IMG img, VOID *v){
	//printf("image name : %s\n", IMG_Name(img).c_str());
	if (process_start != 0) return;
        
        if (strstr(IMG_Name(img).c_str(), KnobProcessToTrace.Value().c_str())){
                process_start = IMG_LowAddress(img);
                process_end   = IMG_HighAddress(img);
        } 
	

        //printf("Loading %.08x-%.08x : %s\n", 	IMG_LowAddress(img),
        //					IMG_HighAddress(img),
        //					IMG_Name(img).c_str());
}

VOID Fini(int, VOID * v){
        int     fd;
        
        fd = open("/tmp/pin_shared_mem", O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
        if (fd == -1){
                perror("open");
                return;
        }
        
        write(fd, (void*)pcounter, SECTION_SIZE);
        close(fd);
}

int main(int argc, char *argv[]){
	int	secfd;
        int	maxhandle;
        
        PIN_InitSymbols();
        if (PIN_Init(argc, argv)) return 1;
	
	//printf("Tracing process : %s\n", KnobProcessToTrace.Value().c_str());
	pcounter = (unsigned char *)malloc(SECTION_SIZE);
	memset(pcounter, 0, SECTION_SIZE);
	
	PIN_AddFiniFunction(Fini, 0);
	
	TRACE_AddInstrumentFunction(Trace, 0);
	IMG_AddInstrumentFunction(ImageLoad, 0);
        PIN_StartProgram();         
        return 0;
}
