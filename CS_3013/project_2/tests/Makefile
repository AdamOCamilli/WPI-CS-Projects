all: testcalls procAncestry

testcalls: testcalls.o
	gcc -g testcalls.o -o testcalls

testcalls.o: testcalls.c
	gcc -g -c testcalls.c

procAncestry: procAncestry.o
	gcc -g procAncestry.o -o procAncestry

procAncestry.o: procAncestry.c
	gcc -g -c procAncestry.c

clean:
	rm -f *.o
	rm -f testcalls procAncestry
