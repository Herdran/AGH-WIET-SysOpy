CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f main

test:
	make main
	./main date > results2.txt
	printf "\n============================================\n\n" >> results2.txt
	./main "stuff" "stuff" jakub@ubuntu.org >> results2.txt
	make clean