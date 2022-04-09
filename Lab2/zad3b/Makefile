CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) directory_search_nftw.c -o directory_search_nftw.o


clean:
	rm -f *.o

test:
	make main
	./directory_search_nftw.o ./aa > pomiar_zad_3b.txt
	make clean