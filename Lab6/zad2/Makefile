CC = gcc
CFLAGS = -g -Wall

all:
	$(CC) $(CFLAGS) server.c -o server -lrt
	$(CC) $(CFLAGS) client.c -o client -lrt

clean:
	rm -f server client
