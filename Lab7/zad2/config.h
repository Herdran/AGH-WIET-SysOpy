#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define N 3 //cookers
#define M 4 //suppliers

#define oven_size 5
#define table_size 5

typedef struct memory_str {
    int oven_index;
    int table_index_cooker;
    int table_index_supplier;
    int pizzas_oven[oven_size];
    int pizzas_table[table_size];
} memory_str;