#include "library.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>

clock_t start_time, end_time;
struct tms start_tms, end_tms;

void start_time_measurement(){
    start_time = times(&start_tms);
}

void end_time_measurement_and_print_results(char *name){
    end_time = times(&end_tms);
    int ticks_per_sec = sysconf(_SC_CLK_TCK);

    double real_time = (double)(end_time - start_time) / ticks_per_sec;
    double user_time = (double)(end_tms.tms_utime - start_tms.tms_utime) / ticks_per_sec;
    double sys_time = (double)(end_tms.tms_stime - start_tms.tms_stime) / ticks_per_sec;

    printf("%49s: \t%20f\t%20f\t%20f\n", name, real_time, user_time, sys_time);
}

void print_header(){
    printf("%49s: \t%20s\t%20s\t%20s\n\n", "type of operation", "real time[s]", "user time[s]", "system time[s]");
}

int main(int argc, char* argv[]){
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "create_array") == 0){
            int size_ = atoi(argv[++i]);
            create_array(size_);
        }

        else if (strcmp(argv[i], "wc_files") == 0){
            wc_files(argv[++i]);
        }

        else if (strcmp(argv[i], "remove_block") == 0){
            remove_block(atoi(argv[++i]));
        }

        else if (strcmp(argv[i], "start_time_measurement") == 0){
            start_time_measurement();
        }

        else if (strcmp(argv[i], "end_time_measurement") == 0){
            end_time_measurement_and_print_results(argv[++i]);
        }

        else if (strcmp(argv[i], "print_header") == 0){
            print_header();
        }

        else if (strcmp(argv[i], "remove_array") == 0){
            remove_array();
        }

        else {
            printf("Unknown command\n\n");
            exit(0);
        }
    }
}