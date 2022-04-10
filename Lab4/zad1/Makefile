CC = gcc
CFLAGS = -g -Wall

main:
	$(CC) $(CFLAGS) main.c -o main.o

main_exec:
	$(CC) $(CFLAGS) main_exec.c -o main_exec.o

clean:
	rm -f *.o

test_main:
	make main
	./main.o handler
	./main.o ignore
	./main.o mask
	./main.o pending
	make clean

test_main_exec:
	make main_exec
	./main_exec.o ignore parent
	./main_exec.o mask parent
	./main_exec.o pending parent
	make clean