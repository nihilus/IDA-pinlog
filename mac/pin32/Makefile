PINPATH=$(HOME)/pin
INCLUDE=-I$(PINPATH)/source/include/pin -I$(PINPATH)/extras/components/include -I$(PINPATH)/source/include/pin/gen -I$(PINPATH)/extras/xed2-ia32/include
LIBPATH=-L$(PINPATH)/ia32/lib-ext -L$(PINPATH)/extras/xed2-ia32/lib/ -L$(PINPATH)/ia32/lib
LINKFLAGS=-m32 -shared -Wl,-exported_symbols_list -Wl,$(PINPATH)/source/include/pin/pintool.exp
VERSION=#-Wl #,--version-script=$(PINPATH)/source/include/pin/pintool.ver
LIBS=-lpin -lxed #-lxed -ldwarf -lelf -ldl -lrt
CXXFLAGS= -m32 -c -fPIC 
DEFINES = -MMD -DBIGARRAY_MULTIPLIER=1 -DUSING_XED -DHOST_IA32 -DTARGET_MAC -DTARGET_IA32

all: trace.dylib
clean: 
	rm -rf trace.o
	rm -rf trace.dylib
	rm -rf trace.d
trace.dylib : trace.o
	g++ $(LIBPATH) $(VERSION) $(LINKFLAGS) trace.o -o trace.dylib $(LIBS)
	rm -rf trace.d
	rm -rf trace.o
trace.o : trace.cpp
	g++ $(DEFINES) $(CXXFLAGS) $(INCLUDE) trace.cpp -o trace.o
	

