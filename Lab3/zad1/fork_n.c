#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf("Needs number of forks to work\n");
        return 1;
    }

    for (int i = 1; i <= atoi(argv[1]); i++) {
        if(fork() == 0) {
            printf("Proces dziecka nr: %d, ma pid:%d\n", i, (int)getpid());
            return 0;
        }
    }
    return 0;
}