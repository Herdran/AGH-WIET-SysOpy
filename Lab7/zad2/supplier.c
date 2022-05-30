#include <time.h>
#include "config.h"


int main() {
    srand(getpid());

    sem_t* table_space = sem_open("/table_space", O_RDWR, 0666);
    sem_t* ready = sem_open("/ready", O_RDWR, 0666);

    int shared_memory = shm_open("/shared_memory", O_RDWR, 0666);

    while(1) {
        sem_wait(ready);
        sem_post(table_space);
        memory_str *m = mmap(NULL, sizeof(memory_str), PROT_WRITE, MAP_SHARED, shared_memory, 0);

        int n = m->pizzas_table[m->table_index_supplier];
        m->table_index_supplier = (m->table_index_supplier + 1) % table_size;

        int pizzas_on_the_table;
        sem_getvalue(ready, &pizzas_on_the_table);
        munmap(m, sizeof(memory_str));

        printf("(%d %lu) Pobieram pizze: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), n, pizzas_on_the_table);
        sleep((rand() % 2) + 4);

        printf("(%d %lu) Dostarczam pizze: %d. \n", getpid(), time(NULL), n);
        sleep((rand() % 2) + 4);
    }
    return 0;
}