#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define N 3 //cookers
#define M 4 //suppliers

#define oven_size 5
#define table_size 5

#define oven_space_index 0
#define table_space_index 1
#define ready_index 2
#define can_use_oven 3

typedef struct memory_str {
    int oven_index;
    int table_index_cooker;
    int table_index_supplier;
    int pizzas_oven[oven_size];
    int pizzas_table[table_size];
} memory_str;
