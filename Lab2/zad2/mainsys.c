#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <ctype.h>
#include <fcntl.h>
#include "sys/stat.h"
#include <stdbool.h>


clock_t start_time, end_time;
struct tms start_tms, end_tms;


void start_time_measurement(){
    start_time = times(&start_tms);
}

void end_time_measurement_and_print_results(){
    end_time = times(&end_tms);
    int ticks_per_sec = sysconf(_SC_CLK_TCK);

    double real_time = (double)(end_time - start_time) / ticks_per_sec;
    double user_time = (double)(end_tms.tms_utime - start_tms.tms_utime) / ticks_per_sec;
    double sys_time = (double)(end_tms.tms_stime - start_tms.tms_stime) / ticks_per_sec;

    printf("real time: %fs user time: %fs system time: %fs\n", real_time, user_time, sys_time);
}


int main(int argc, char* argv[]){
    if (argc > 3) {
        printf("Too many arguments");
        return 0;
    }
    else if (argc == 2) {
        printf("Need two files to work\n");
        return 0;
    }

    char searched_char[1];
    char* source_path = calloc(100, 1);

    if (argc < 3) {
        scanf("%c %s", searched_char, source_path);
    }
    else {
        strcpy(searched_char, argv[1]);
        source_path = argv[2];
    }

    start_time_measurement();

    int source = open(source_path, O_RDONLY);

    struct stat st;
    stat(source_path, &st);
    off_t file_len = st.st_size;
    char source_content[file_len];
    read(source, source_content, file_len);

    int char_counter = 0;
    int char_lines_counter = 0;
    bool new_line = false;


    for (int i = 0; i < file_len; i++){
        if (source_content[i] == *searched_char) {
            char_counter++;
            if (!new_line){
                char_lines_counter++;
                new_line = true;
            }
        }
        if (source_content[i] == '\n'){
            new_line = false;
        }
    }
    printf("Character \'%c\'  appears %d times in %d lines in a given file\n", *searched_char, char_counter, char_lines_counter);
    close(source);
    end_time_measurement_and_print_results();
}