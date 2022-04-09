CC = gcc
CFLAGS = -g -Wall

fork_n:
	$(CC) $(CFLAGS) fork_n.c -o fork_n.o

clean:
	rm -f *.o

test:
	make fork_n
	./fork_n.o 3
	make clean