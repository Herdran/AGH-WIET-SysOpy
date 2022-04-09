#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <ctype.h>
#include <fcntl.h>
#include "sys/stat.h"


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

    char* source_path = calloc(100, 1);
    char* target_path = calloc(100, 1);
    if (argc < 3) {
        scanf("%s%s", source_path, target_path);
    }
    else {
        strcpy(source_path,argv[1]);
        strcpy(target_path,argv[2]);
    }

    start_time_measurement();

    int source = open(source_path, O_RDONLY);
    int target = open(target_path, O_WRONLY);

    struct stat st;
    stat(source_path, &st);
    off_t file_len = st.st_size;
    char source_content[file_len];
    read(source, source_content, file_len);

    char* target_content = calloc(file_len, 1);

    int source_index = 0;
    int target_index=0;

    for (int i = 0; i < file_len; i++){
        if (isspace(source_content[i])) {
            source_index++;
        }
        else {
            while (source_index <= i) {
                target_content[target_index] = source_content[source_index];
                source_index++;
                target_index++;
            }
        }
    }
    write(target, target_content, target_index);

    close(source);
    close(target);
    end_time_measurement_and_print_results();
}