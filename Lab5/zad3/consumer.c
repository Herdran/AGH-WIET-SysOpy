#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#define MAX_PRODUCER 10
#define MAX_LINE_LEN 200

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Wrong number of arguments!");
        exit(1);
    }


    FILE *pipe = fopen(argv[1], "r");
    FILE *file = fopen(argv[2], "w+");
    const int N = atoi(argv[3]);

    char *buffer = calloc(N + 3, sizeof(char));
    char array[MAX_PRODUCER][MAX_LINE_LEN] = {0};

    int no_line, max_row = 0;
    while(fscanf(pipe, "%d|%s\n", &no_line, buffer) != EOF){
        strcat(array[no_line], buffer);
        if(no_line > max_row){
            max_row = no_line;
        }
    }

    for(int i = 0; i <= max_row; i++) {
        fprintf(file, "%s\n", array[i]);
    }


    free(buffer);
    fclose(file);
    fclose(pipe);
    exit(0);
}