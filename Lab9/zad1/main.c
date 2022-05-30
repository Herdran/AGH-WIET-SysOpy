#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define ELVES_NUMBER 10
#define REINDEER_NUMBER 9

int santa_sleeping;
int send_presents;
int reindeers_ready;
int elves_help;
int elves[ELVES_NUMBER];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeers_busy = PTHREAD_COND_INITIALIZER;

void help_elves() {
    int first = -1, second = -1;

    for(int i = 0; i < ELVES_NUMBER; i++) {
        if(elves[i] == 1) {
            if (first == -1){
                first = i+1;
            }
            else if (second == -1){
                second = i+1;
            }
            else{
                printf("Mikołaj: rozwiązuje problemy elfów %d %d %d\n", first, second, i + 1);
            }
            elves[i] = 0;
        }
    }
}

void santa() {
    while (true) {
        pthread_mutex_lock(&mutex);
        if(reindeers_ready < REINDEER_NUMBER && elves_help < 3) {
            santa_sleeping = 1;
            pthread_cond_wait(&santa_ready, &mutex);
        }
        printf("Mikolaj: budzę się\n");
        printf("%d %d\n", reindeers_ready, elves_help);
        santa_sleeping = 0;

        if(reindeers_ready >= REINDEER_NUMBER) {
            printf("Mikolaj: dostarczam zabawki\n");
            reindeers_ready = 0;
            send_presents++;
            sleep(rand() % (4 + 1 - 2) + 2);
            printf("Mikolaj: zasypiam\n");
            pthread_cond_broadcast(&reindeers_busy);
        }
        if (elves_help == 3) {
            help_elves();
            elves_help = 0;
            sleep(rand() % (2 + 1 - 1) + 1);
            printf("Mikolaj: zasypiam\n");
            pthread_cond_broadcast(&queue_space);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void reindeer(int* id) {
    sleep(rand() % (10 + 1 - 5) + 5);
    while(true) {
        pthread_mutex_lock(&mutex);
        if(reindeers_ready >= REINDEER_NUMBER-1) {
            if (reindeers_ready == REINDEER_NUMBER-1){
                reindeers_ready++;
                printf("Renifer: wybudzam Mikołaja\t%d\n", *id);
                pthread_cond_broadcast(&santa_ready);
            }
        }
        else {
            reindeers_ready++;
            printf("Renifer: czeka %d reniferów na Mikołaja\t%d\n", reindeers_ready, *id);
        }
        pthread_cond_wait(&reindeers_busy, &mutex);
        pthread_mutex_unlock(&mutex);
    }
}

void elv(int* id) {
    int index = *id;
    index--;

    while(true) {
        bool need_help = rand() % 10 < 3;
        if(need_help) {
            pthread_mutex_lock(&mutex);
            if (elves_help == 3){
//                printf("\n\n\n");
                printf("Elf: czeka na powrót elfów\t%d\n", *id);
                pthread_cond_wait(&queue_space, &mutex);
                while (elves_help == 3) {
                    pthread_cond_wait(&queue_space, &mutex);
                }
            }

            if(elves_help < 2) {
                elves_help++;
                elves[index] = 1;
                printf("Elf: czeka %d elfów na Mikołaja\t%d\n", elves_help, *id);
                pthread_cond_wait(&queue_space, &mutex);
            }
            else if(elves_help == 2) {
                elves_help++;
                elves[index] = 1;
                printf("Elf: wybudzam Mikołaja\t%d\n", *id);
                pthread_cond_broadcast(&santa_ready);
                pthread_cond_wait(&queue_space, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        sleep(rand() % (5 + 1 - 2) + 2);
    }
}

int main() {
    srand(time(NULL));
    santa_sleeping = 0;
    send_presents = 0;
    reindeers_ready = 0;
    elves_help = 0;

    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, (void* (*)(void*))santa, NULL);

    pthread_t *reindeers = calloc(REINDEER_NUMBER, sizeof(pthread_t));
    int* ids_rein = calloc(ELVES_NUMBER, sizeof(int));
    for(int i = 0; i < REINDEER_NUMBER; i++) {
        ids_rein[i] = i + 1;
        pthread_create(&reindeers[i], NULL, (void* (*)(void*))reindeer, ids_rein + i);
    }

    pthread_t *elves_th = calloc(ELVES_NUMBER, sizeof(pthread_t));
    int* ids_elves = calloc(ELVES_NUMBER, sizeof(int));
    for(int i = 0; i < ELVES_NUMBER; i++) {
        ids_elves[i] = i + 1;
        pthread_create(&elves_th[i], NULL, (void* (*)(void*))elv, ids_elves + i);
    }

    while(send_presents < 3) {}

    free(reindeers);
    free(elves_th);
    free(ids_elves);
    free(ids_rein);

    return 0;
}