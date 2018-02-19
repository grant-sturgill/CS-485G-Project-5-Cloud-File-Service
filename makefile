all: client server

client: echoclient.c csapp.c
	gcc -o myclient echoclient.c csapp.c -lpthread

server: echoserveri.c csapp.c
	gcc -o myserver echoserveri.c csapp.c -lpthread
