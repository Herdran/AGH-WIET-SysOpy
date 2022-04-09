CC = gcc
CFLAGS = -g -Wall

integrate:
	$(CC) $(CFLAGS) integrate.c -o integrate.o

clean:
	rm -f *.o

test:
	printf "tezd:\n" > results2.txt
	make integrate
	./integrate.o 100 3 >> results2.txt
	./integrate.o 5 5 >> results2.txt
	./integrate.o 10 10 >> results2.txt
	./integrate.o 30 30 >> results2.txt
	./integrate.o 50 50 >> results2.txt
	./integrate.o 80 80 >> results2.txt
	./integrate.o 100 100 >> results2.txt
	./integrate.o 200 200 >> results2.txt
	make clean
