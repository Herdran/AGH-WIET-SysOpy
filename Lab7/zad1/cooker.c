#include <time.h>
#include "config.h"


int main() {
    srand(getpid());
    key_t key = ftok("main", 1);

    int semaphores = semget(key, 4, 0);
    int shared_memory = shmget(key, sizeof(memory_str), 0);

    struct sembuf lock_memory = {can_use_oven, -1, 0};
    struct sembuf unlock_memory = {can_use_oven, 1, 0};

    struct sembuf decrement_space = {oven_space_index, -1, 0};
    struct sembuf increment_space = {oven_space_index, 1, 0};

    struct sembuf decrement_space_table = {table_space_index, -1, 0};
    struct sembuf increment_ready_pizzas = {ready_index, 1, 0};

    struct sembuf start[2] = {lock_memory, decrement_space};
//    struct sembuf taking_pizza_out[2] = {lock_memory, decrement_space_table};
    struct sembuf end[3] = {increment_space, increment_ready_pizzas, unlock_memory};

    while(1) {
        int n = rand() % 10;
        printf("(%d %lu) Przygotowuje pizze: %d.\n", getpid(), time(NULL), n);
        sleep((rand() % 2) + 1);

        semop(semaphores, start, 2);
        memory_str *m = shmat(shared_memory, NULL, 0);

        m->pizzas_oven[m->oven_index] = n;
        m->oven_index = (m->oven_index + 1) % oven_size;

        int pizzas_in_the_oven = oven_size - semctl(semaphores, oven_space_index, GETVAL);

        printf("(%d %lu) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), time(NULL), n, pizzas_in_the_oven);

        semop(semaphores, &unlock_memory, 1);
        sleep((rand() % 2) + 4);

//        To avoid burning pizza if there is no place on the table the cooker will take it out and hold it "in hands" waiting for space on the table
        semop(semaphores, &lock_memory, 1);
        semop(semaphores, &decrement_space_table, 1);

//        semop(semaphores, taking_pizza_out, 2);

        m->pizzas_table[m->table_index_cooker] = n;
        m->table_index_cooker = (m->table_index_cooker + 1) % table_size;

        pizzas_in_the_oven = table_size - semctl(semaphores, oven_space_index, GETVAL);

        int pizzas_on_the_table = semctl(semaphores, ready_index, GETVAL);

        printf("(%d %lu) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), n, pizzas_in_the_oven-1, pizzas_on_the_table+1);

        semop(semaphores, end, 3);
        shmdt(m);
    }
    return 0;
}