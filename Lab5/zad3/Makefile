CC = gcc
CFLAGS = -Wall -g

compile:
	$(CC) $(FLAGS) main.c -o main
	$(CC) $(FLAGS) consumer.c -o consumer
	$(CC) $(FLAGS) producer.c -o producer

clean:
	rm -f main consumer producer pipe

test:
	make compile
	./main
	make clean
