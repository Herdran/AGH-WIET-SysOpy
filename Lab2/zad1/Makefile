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
	printf "Lib small file size measurements:\n" > pomiar_zad_1.txt
	./mainlib.o "test_files/small.txt" "test_files/small_no_whitespaces.txt" >> pomiar_zad_1.txt
	printf "\nLib big file size measurements:\n" >> pomiar_zad_1.txt
	./mainlib.o "test_files/big.txt" "test_files/big_no_whitespaces.txt" >> pomiar_zad_1.txt
	printf "\n" >> pomiar_zad_1.txt
	make main_sys
	printf "\nSys small file size measurements:\n" >> pomiar_zad_1.txt
	./mainsys.o "test_files/small.txt" "test_files/small_no_whitespaces.txt" >> pomiar_zad_1.txt
	printf "\nSys big file size measurements:\n" >> pomiar_zad_1.txt
	./mainsys.o "test_files/big.txt" "test_files/big_no_whitespaces.txt" >> pomiar_zad_1.txt
	make clean

test_noargs:
	make main_lib
	printf "Lib measurements:\n" > pomiar_zad_1.txt
	./mainlib.o >> pomiar_zad_1.txt
	printf "\n" >> pomiar_zad_1.txt
	make main_sys
	printf "Sys measurements" >> pomiar_zad_1.txt
	./mainsys.o >> pomiar_zad_1.txt
	make clean
