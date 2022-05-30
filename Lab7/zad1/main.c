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
    key_t key = ftok("main", 1);

    int semaphores = semget(key, 4, IPC_CREAT | 0666);
    semctl(semaphores, oven_space_index, SETVAL, oven_size);
    semctl(semaphores, table_space_index, SETVAL, table_size);
    semctl(semaphores, ready_index, SETVAL, 0);
    semctl(semaphores, can_use_oven, SETVAL, 1);

    int shared_memory = shmget(key, sizeof(memory_str), IPC_CREAT | 0666);
    memory_str *m = shmat(shared_memory, NULL, 0);

    m->oven_index = 0;
    m->table_index_cooker = 0;
    m->table_index_supplier = 0;

    shmdt(m);

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

    semctl(semaphores, 0, IPC_RMID);
    shmctl(shared_memory, IPC_RMID, NULL);

    return 0;
}