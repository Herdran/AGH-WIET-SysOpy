#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



static void handleSignal(int sigNum){
    if(sigNum == SIGUSR1) {
        printf("Received the signal\n");
    }
    else {
        printf("Received signal %d\n", sigNum);
    }
}


void maskSignal(){
    struct sigaction act;
    act.sa_handler = handleSignal;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);

    if(sigprocmask(SIG_BLOCK, &act.sa_mask, NULL) < 0) {
        perror("Couldn't block the signal\n");
    }
}


void checkIfPending(){

    sigset_t pendingMask;
    sigpending(&pendingMask);

    if(sigismember(&pendingMask, SIGUSR1) == 1) {
        printf("Signal is pending\n");
    }
    else {
        printf("Signal is not pending\n");
    }
}


int main(int argc, char *argv[]){

    if(argc != 2) {
        printf("Wrong amount of arguments\n");
        exit(1);
    }

    if(strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        if (fork() == 0)
            raise(SIGUSR1);
        else
            wait(NULL);
    }

    else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handleSignal);
        raise(SIGUSR1);
        if (fork() == 0)
            raise(SIGUSR1);
        else
            wait(NULL);
    }

    else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        maskSignal();
        raise(SIGUSR1);
        checkIfPending();
        if (strcmp(argv[1], "pending") == 0) {
            if (fork() == 0)
                checkIfPending();
            else
                wait(NULL);
        }
    }



    while(wait(NULL) > 0);
    return 0;
}

