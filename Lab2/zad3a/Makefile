CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) directory_search.c -o directory_search.o


clean:
	rm -f *.o

test:
	make main
	./directory_search.o ./aa > pomiar_zad_3a.txt
	make clean

