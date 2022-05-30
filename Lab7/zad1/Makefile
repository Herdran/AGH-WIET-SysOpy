CC = gcc
CFLAGS = -g -Wall

build:
	$(CC) -o main main.c
	$(CC) -o cooker cooker.c
	$(CC) -o supplier supplier.c

test: build
	./main

clean:
	rm -f main cooker supplier