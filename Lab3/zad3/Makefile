CC = gcc
CFLAGS = -g -Wall

directory_search_fork:
	$(CC) $(CFLAGS) directory_search_fork.c -o directory_search_fork.o

clean:
	rm -f *.o

test:
	make directory_search_fork
	./directory_search_fork.o ./aa "1.1.1" 2 > results3.txt
	make clean