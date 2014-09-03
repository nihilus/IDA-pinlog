PINPATH=$(HOME)/Desktop/pin
INCLUDE=-I$(PINPATH)/source/include/pin -I$(PINPATH)/extras/components/include -I$(PINPATH)/source/include/pin/gen -I$(PINPATH)/extras/xed2-ia32/include
LIBPATH=-L$(PINPATH)/ia32/lib-ext -L$(PINPATH)/extras/xed2-ia32/lib/ -L$(PINPATH)/ia32/lib
LINKFLAGS=-m32 -Wl,--hash-style=sysv -Wl,-Bsymbolic -shared -fno-strict-aliasing -Wall -Werror -fno-stack-protector -fomit-frame-pointer -O3
VERSION=-Wl,--version-script=$(PINPATH)/source/include/pin/pintool.ver
LIBS=-lpin -lxed -ldwarf -lelf -ldl -lrt
CXXFLAGS= -m32 -c -shared -fPIC 
DEFINES = -MMD -DBIGARRAY_MULTIPLIER=1 -DUSING_XED -DHOST_IA32 -DTARGET_LINUX -DTARGET_IA32

all: trace.so
clean: 
	rm -rf trace.o
	rm -rf trace.so	
	rm -rf trace.d
trace.so : trace.o
	g++ $(LIBPATH) $(VERSION) $(LINKFLAGS) trace.o -o trace.so $(LIBS)
	rm -rf trace.d
	rm -rf trace.o
trace.o : trace.cpp
	g++ $(DEFINES) $(CXXFLAGS) $(INCLUDE) trace.cpp -o trace.o
	

