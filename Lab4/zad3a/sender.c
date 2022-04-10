#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

int total;
int countSignals = 0;
int handleSignals = 1;
int catcherGot = 0;
int sq = 0;

void increment(int sig_no, siginfo_t *info, void *ucontext) {
    if (info->si_code == SI_QUEUE) {
        catcherGot = info->si_value.sival_int;
        printf("%d\n", catcherGot);
    }
    countSignals++;
}

void finish(int sig_no) {
    printf("Number of received signals: %d\n", countSignals);
    printf("Should have received: %d\n", total);
    if (sq) {
        printf("Catcher received: %d\n", catcherGot);
    }
    handleSignals = 0;
}

int main (int argc, char* argv[]) {
    if (argc != 4) {
        printf("Wrong amount of arguments\n");
        exit(1);
    }

    int pid = atoi(argv[1]);
    total = atoi(argv[2]);

    if (strcmp(argv[3], "KILL") == 0) {
        for (int i = 0; i < total; i++) {
            kill(pid, SIGUSR1);
        }
        kill(pid, SIGUSR2);
    }

    else if (strcmp(argv[3], "SIGQUEUE") == 0) {
        union sigval value;
        sq = 1;

        for (int i = 0; i < total; i++) {
            value.sival_int = i;
            sigqueue(pid, SIGUSR1, value);
        }
        kill(pid, SIGUSR2);
    }

    else if (strcmp(argv[3], "SIGRT") == 0) {
        for (int i = 0; i < total; i++) {
            kill(pid, SIGRTMIN);
        }
        kill(pid, SIGRTMIN+1);
    }

    struct sigaction act;
    act.sa_sigaction = increment;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    signal(SIGUSR2, finish);

    sigaction(SIGRTMIN, &act, NULL);
    signal(SIGRTMIN+1, finish);

    while(handleSignals);
    return 0;
}