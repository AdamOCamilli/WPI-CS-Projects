all: refactor_server refactor_dsh

refactor_server: refactor_server.c 
	gcc -g refactor_server.c -o refactor_server -lm -lcrypt 

refactor_dsh: refactor_dsh.c
	gcc -g refactor_dsh.c -o refactor_dsh -lm -lcrypt 

clean:
	rm -f *.o
	rm refactor_server
	rm refactor_dsh
