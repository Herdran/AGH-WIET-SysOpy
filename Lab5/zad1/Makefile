CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f main

test:
	make main
	./main input.txt > results1.txt
	make clean