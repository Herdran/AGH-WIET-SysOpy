#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>

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


int main (int argc, char *argv[]) {
    if (argc < 3) {
        printf("Needs more arguments to function\n");
        return 1;
    }

    start_time_measurement();

    double width = 1.0/atoi(argv[1]);
    int processes = atoi(argv[2]);

    for (int i = 0; i < processes; i++) {
        if (fork() == 0) {
            double area = 0.0;

            for (double x = (double) i / processes; x < (double) (i + 1)/processes; x += width) {
                area += (4*width)/(x*x + 1);
            }


            char filename[10];
            snprintf(filename, sizeof(filename), "w%d.txt", i+1);
            FILE* file = fopen(filename, "w");
            fprintf(file, "%f", area);
            fclose(file);
            return 0;
        }
    }


    for (int i = 0; i < processes; i++) {
        wait(0);
    }

    double area = 0.0;
    double total_area = 0.0;

    for (int i = 0; i < processes; i++) {
        char filename[10];
        snprintf(filename, sizeof(filename), "w%d.txt", i+1);
        FILE* file = fopen(filename, "r");
        fscanf(file, "%lf", &area);
        fclose(file);
        remove(filename);
        total_area += area;
    }

    printf("%f\n", total_area);
    end_time_measurement_and_print_results();
    return 0;
}