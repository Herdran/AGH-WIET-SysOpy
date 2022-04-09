CC = gcc
CFLAGS = -g -Wall

catcher:
	$(CC) $(CFLAGS) catcher.c -o catcher.o

sender:
	$(CC) $(CFLAGS) sender.c -o sender.o

clean:
	rm -f *.o

test_catcher:
	make catcher
	./catcher.o KILL
	./catcher.o QUEUE
	./catcher.o SIGRT
	make clean

test_sender:
	make sender
	./sender.o 0 1000 KILL
	./sender.o 0 1000 QUEUE
	./sender.o 0 1000 SIGRT
	make clean


