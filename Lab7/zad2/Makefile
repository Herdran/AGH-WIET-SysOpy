CC = gcc
CFLAGS = -g -Wall

build:
	$(CC) -o main main.c -lrt -lpthread
	$(CC) -o cooker cooker.c -lrt -lpthread
	$(CC) -o supplier supplier.c -lrt -lpthread

test: build
	./main

clean:
	rm -f main cooker supplier