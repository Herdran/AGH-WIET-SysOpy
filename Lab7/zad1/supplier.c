#include <time.h>
#include "config.h"


int main() {
    srand(getpid());
    key_t key = ftok("main", 1);

    int semaphores = semget(key, 4, 0);
    int shared_memory = shmget(key, sizeof(memory_str), 0);

    struct sembuf increment_space_table = {table_space_index, 1, 0};
    struct sembuf decrement_ready_pizzas = {ready_index, -1, 0};

    struct sembuf start[2] = {increment_space_table, decrement_ready_pizzas};


    while(1) {
        semop(semaphores, start, 2);
        memory_str *m = shmat(shared_memory, NULL, 0);

        int n = m->pizzas_table[m->table_index_supplier];
        m->table_index_supplier = (m->table_index_supplier + 1) % table_size;

        int pizzas_on_the_table = semctl(semaphores, ready_index, GETVAL);
        shmdt(m);

        printf("(%d %lu) Pobieram pizze: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), n, pizzas_on_the_table);
        sleep((rand() % 2) + 4);

        printf("(%d %lu) Dostarczam pizze: %d. \n", getpid(), time(NULL), n);
        sleep((rand() % 2) + 4);
    }
    return 0;
}