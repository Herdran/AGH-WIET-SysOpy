#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#define FIFO_NAME "/tmp/pipe"

int main() {
    mkfifo(FIFO_NAME, S_IRUSR | S_IWUSR);
    char* input_files[] = {"producer_files/1.txt", "producer_files/2.txt", "producer_files/A.txt", "producer_files/B.txt", "producer_files/C.txt"};


    for (int i = 0; i < 5; i++){
        char row[2];
        sprintf(row,"%d", i);
        if (fork() == 0) {
            execlp("./producer", "./producer", FIFO_NAME, input_files[i], row, "10", NULL);
        }
    }

    if (fork() == 0) {
        execlp("./consumer", "./consumer", FIFO_NAME, "result.txt", "10", NULL);
    }

    wait(0);
    return 0;
}