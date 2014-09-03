
all: pinlog
clean: 
	rm -rf pinlog
	rm -rf pin32/trace.so
	rm -rf pin64/trace.so

pinlog : pinlog.c
	gcc pinlog.c -o pinlog -lrt

