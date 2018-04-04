all: server client

server: server.c
	gcc -g -pthread server.c -o server

client: client.c
	gcc -g -pthread client.c -o client

clean:
	rm -f *.o
	rm server
	rm client
