#include <time.h>
#include "config.h"


int main() {
    srand(getpid());

    sem_t* oven_space = sem_open("/oven_space", O_RDWR, 0666);
    sem_t* table_space = sem_open("/table_space", O_RDWR, 0666);
    sem_t* ready = sem_open("/ready", O_RDWR, 0666);
    sem_t* can_modify = sem_open("/can_modify", O_RDWR, 0666);

    int shared_memory = shm_open("/shared_memory", O_RDWR, 0666);

    while(1) {
        int n = rand() % 10;
        printf("(%d %lu) Przygotowuje pizze: %d.\n", getpid(), time(NULL), n);
        sleep((rand() % 2) + 1);


        sem_wait(oven_space);
        sem_wait(can_modify);

        memory_str *m =  mmap(NULL, sizeof(memory_str), PROT_WRITE, MAP_SHARED, shared_memory, 0);

        m->pizzas_oven[m->oven_index] = n;
        m->oven_index = (m->oven_index + 1) % oven_size;

        int pizzas_in_the_oven;
        sem_getvalue(oven_space, &pizzas_in_the_oven);
        pizzas_in_the_oven = oven_size - pizzas_in_the_oven;

        printf("(%d %lu) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), time(NULL), n, pizzas_in_the_oven);

        sem_post(can_modify);
        sleep((rand() % 2) + 4);

//        To avoid burning pizza if there is no place on the table the cooker will take it out and hold it "in hands" waiting for space on the table
        sem_wait(can_modify);
        sem_wait(table_space);

        m->pizzas_table[m->table_index_cooker] = n;
        m->table_index_cooker = (m->table_index_cooker + 1) % table_size;

        sem_getvalue(oven_space, &pizzas_in_the_oven);
        pizzas_in_the_oven = oven_size - pizzas_in_the_oven;

        int pizzas_on_the_table;
        sem_getvalue(ready, &pizzas_on_the_table);

        printf("(%d %lu) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), n, pizzas_in_the_oven-1, pizzas_on_the_table+1);

        sem_post(oven_space);
        sem_post(ready);
        sem_post(can_modify);
        munmap(m, sizeof(memory_str));
    }
    return 0;
}