all: rm dump dv

rm: rm.c dumpster.o dumpster.h
	gcc -o rm dumpster.o rm.c -g 

dump: dump.c dumpster.o dumpster.h
	gcc -o dump dumpster.o dump.c -g

dv: dv.c dumpster.o dumpster.h
	gcc -o dv dumpster.o dv.c -g

clean: 
	rm -f *.o
	rm -f rm dump dv

