#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include "config.h"

pid_t children_pids[N + M];

void quit_handler() {
    for(int i = 0; i < N + M; i++) {
        kill(children_pids[i], SIGKILL);
    }
}

int main() {
    signal(SIGINT, quit_handler);

    sem_t* oven_space = sem_open("/oven_space", O_CREAT | O_RDWR, 0666, oven_size);
    sem_t* table_space = sem_open("/table_space", O_CREAT | O_RDWR, 0666, table_size);
    sem_t* ready = sem_open("/ready", O_CREAT | O_RDWR, 0666, 0);
    sem_t* can_modify = sem_open("/can_modify", O_CREAT | O_RDWR, 0666, 1);

    int shared_memory = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shared_memory, sizeof(memory_str));

    memory_str *m = mmap(NULL, sizeof(memory_str), PROT_WRITE, MAP_SHARED, shared_memory, 0);

    m->oven_index = 0;
    m->table_index_cooker = 0;
    m->table_index_supplier = 0;

    munmap(m, sizeof(memory_str));

    for (int i = 0; i < N; i++) {
        children_pids[i] = fork();
        if (children_pids[i] == 0) {
            execlp("./cooker", "./cooker", NULL);
        }
    }

    for (int i = 0; i < M; i++) {
        children_pids[N+i] = fork();
        if (children_pids[N+i] == 0) {
            execlp("./supplier", "./supplier", NULL);
        }
    }

    for (int i = 0; i < N + M; i++) {
        wait(0);
    }

    sem_close(oven_space);
    sem_close(table_space);
    sem_close(ready);
    sem_close(can_modify);

    sem_unlink("/oven_space");
    sem_unlink("/table_space");
    sem_unlink("/ready");
    sem_unlink("/can_modify");

    shm_unlink("/shared_memory");

    return 0;
}