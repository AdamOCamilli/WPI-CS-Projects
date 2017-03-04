all: solution1 solution2

sandbox: sandbox.o
	gcc -g -o sandbox sandbox.c -pthread

sandbox.o: sandbox.c
	gcc -g -c sandbox.c -pthread

solution1: solution1.o
	gcc -g -o solution1 solution1.c -pthread

solution1.o: solution1.c
	gcc -g -c solution1.c -pthread

solution2: solution2.o
	gcc -g -o solution2 solution2.c -pthread

solution2.o: solution2.c
	gcc -g -c solution2.c -pthread

clean:
	rm -f *.o
	rm -f solution1 solution2
