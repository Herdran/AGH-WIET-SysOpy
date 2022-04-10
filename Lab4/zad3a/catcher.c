#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

int countSignals = 0;
int handleSignals = 1;

void increment(int sig_no) {
    countSignals++;
}

void send(int sig_no, siginfo_t *info, void *ucontext) {
    for (int i = 0; i < countSignals; i++) {
        if (info->si_code == SI_QUEUE) {
            union sigval value;
            value.sival_int = i;
            sigqueue(info->si_pid, SIGUSR1, value);
        } else {
            kill(info->si_pid, SIGUSR1);
        }
    }

    kill(info->si_pid, SIGUSR2);
    printf("Number of received signals: %d\n", countSignals);
    handleSignals = 0;
}


int main (int argc, char *argv[]) {
    printf("%d\n", getpid());

    signal(SIGUSR1, increment);

    struct sigaction act;
    act.sa_sigaction = send;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR2, &act, NULL);

    signal(SIGRTMIN, increment);
    sigaction(SIGRTMIN+1, &act, NULL);

    while(handleSignals);
    return 0;
}