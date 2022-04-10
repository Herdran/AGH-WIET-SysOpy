CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) main.c -o main.o

clean:
	rm -f *.o

test:
	make main
	./main.o
	make clean