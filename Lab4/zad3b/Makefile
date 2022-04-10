CC = gcc
CFLAGS = -g -Wall

catcher:
	$(CC) $(CFLAGS) catcher.c -o catcher.o

sender:
	$(CC) $(CFLAGS) sender.c -o sender.o

clean:
	rm -f *.o

test_kill:
	make catcher
	make sender
	./catcher.o KILL
	./sender.o 0 1000 KILL
	make clean

test_queue:
	make catcher
	make sender
	./catcher.o QUEUE
	./sender.o 0 1000 QUEUE
	make clean

test_sigrt:
	make catcher
	make sender
	./catcher.o SIGRT
	./sender.o 0 1000 SIGRT
	make clean