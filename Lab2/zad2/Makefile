CC = gcc
CFLAGS = -g -Wall

main_lib:
	$(CC) $(CFLAGS) mainlib.c -o mainlib.o

main_sys:
	$(CC) $(CFLAGS) mainsys.c -o mainsys.o

clean:
	rm -f *.o

test:
	make main_lib
	printf "Lib small file size measurements:\n" > pomiar_zad_2.txt
	./mainlib.o 'a' "test_files/small.txt" >> pomiar_zad_2.txt
	printf "\nLib big file size measurements:\n" >> pomiar_zad_2.txt
	./mainlib.o 'a' "test_files/big.txt" >> pomiar_zad_2.txt
	printf "\n" >> pomiar_zad_2.txt
	make main_sys
	printf "\nSys small file size measurements:\n" >> pomiar_zad_2.txt
	./mainsys.o 'a' "test_files/small.txt" >> pomiar_zad_2.txt
	printf "\nSys big file size measurements:\n" >> pomiar_zad_2.txt
	./mainsys.o 'a' "test_files/big.txt" >> pomiar_zad_2.txt
	make clean

test_noargs:
	make main_lib
	printf "Lib measurements\n" > pomiar_zad_2.txt
	./mainlib.o >> pomiar_zad_2.txt
	printf "\n" >> pomiar_zad_2.txt
	make main_sys
	printf "Sys measurements" >> pomiar_zad_2.txt
	./mainsys.o >> pomiar_zad_2.txt
	make clean
