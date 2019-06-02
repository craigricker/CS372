CFLAGS= -Wall -fpic -lm -std=gnu99

client: chatclient.c
	gcc chatclient.c -o chatclient $(CFLAGS)
server: ftserver.c
	gcc ftserver.c -o ftserver $(CFLAGS)

all: client server
