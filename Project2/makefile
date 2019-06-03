CFLAGS= -Wall -fpic -lm -std=gnu99

client: 
	chmod +x ftclient.py
server: ftserver.c
	gcc ftserver.c -o ftserver $(CFLAGS)
	chmod +x ftserver

all: client server
