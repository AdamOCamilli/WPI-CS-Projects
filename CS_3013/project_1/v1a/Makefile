all: mc1_a 

mc1: mc1_a.o
	gcc -g mc1_a.o -o mc1_a

mc1.o: mc1_a.c
	gcc -g -c mc1_a.c 

clean:
	rm -f *.o
	rm -f mc1_a 

