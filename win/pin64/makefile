PIN_PATH = $(HOMEPATH)\desktop\pin
DEFINES  = /DTARGET_WINDOWS /DBIGARRAY_MULTIPLIER=1 /DUSING_XED /D_SECURE_SCL=0 /DTARGET_IA32E /DHOST_IA32E /DUNICODE
#INCLUDES = /Ixed-include /Iinclude /Iinclude\gen /Icomponents\include
INCLUDES = /I$(PIN_PATH)/extras/xed2-intel64/include /I$(PIN_PATH)/source/include/pin /I$(PIN_PATH)/source/include/pin/gen /I$(PIN_PATH)/extras/components/include
CFLAGS = /c /MT /EHs- /EHa- /wd4530  /nologo /Gy /Od /Zi 
CPLUSPLUS = /TP
C         = /TC

LINKFLAGS =  /EXPORT:main /DLL /NODEFAULTLIB  /NOLOGO /INCREMENTAL:NO  /MACHINE:x64 /ENTRY:Ptrace_DllMainCRTStartup /BASE:0x55000000
LIBPATHS  =  /LIBPATH:$(PIN_PATH)/intel64/lib /LIBPATH:$(PIN_PATH)/intel64/lib-ext /LIBPATH:$(PIN_PATH)/extras/xed2-intel64/lib /LIBPATH:./
LIBS      =  pin.lib libxed.lib libcpmt.lib libcmt.lib pinvm.lib ntdll-64.lib kernel32.lib

all:    pinme


pinme:
        cl main.cpp $(CPLUSPLUS) $(CFLAGS) $(INCLUDES) $(DEFINES)
        cl log.c  $(CFLAGS) $(CFLAGS) $(INCLIDES) $(DEFINES)
        
        link main.obj log.obj /out:trace.dll $(LINKFLAGS) $(LIBPATHS) $(LIBS) 
        del /q *.obj
        del /q *.pdb
        del /q *.exp
        del /q trace.lib