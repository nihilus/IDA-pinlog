PINPATH=$(HOME)/pin
INCLUDE=-I$(PINPATH)/source/include/pin -I$(PINPATH)/extras/components/include -I$(PINPATH)/source/include/pin/gen -I$(PINPATH)/extras/xed2-intel64/include
LIBPATH=-L$(PINPATH)/intel64/lib-ext -L$(PINPATH)/extras/xed2-intel64/lib/ -L$(PINPATH)/intel64/lib
LINKFLAGS=-shared -Wl,-exported_symbols_list -Wl,$(PINPATH)/source/include/pin/pintool.exp 
VERSION=#-Wl,--version-script=$(PINPATH)/source/include/pintool.ver
LIBS=-lpin -lxed #-ldwarf -lelf -ldl -lrt
CXXFLAGS= -c -fPIC 
DEFINES = -MMD -DBIGARRAY_MULTIPLIER=1 -DUSING_XED -DHOST_IA32E -DTARGET_MAC -DTARGET_IA32E

all: trace.dylib
clean: 
	rm -rf trace.o
	rm -rf trace.so	
	rm -rf trace.d

trace.dylib : trace.o
	g++ $(LIBPATH) $(VERSION) $(LINKFLAGS) trace.o -o trace.dylib $(LIBS)
	rm -rf trace.d
	rm -rf trace.o
	
trace.o : trace.cpp
	g++ $(DEFINES) $(CXXFLAGS) $(INCLUDE) trace.cpp -o trace.o
	

