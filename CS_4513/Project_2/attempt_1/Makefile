all: server dsh

server: server.c 
	gcc -g server.c -o server -lm -lcrypt 

dsh: dsh.c
	gcc -g dsh.c -o dsh -lm -lcrypt 

clean:
	rm -f *.o
	rm server
	rm dsh
