#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/times.h>

#define MAX_LINE_LENGTH 128
#define WHITESPACE_DELIMITER " \t\r\n"

int** image;
int** new_image;

int width;
int height;

int threads_count;
int* intervals;


int calculate_time(struct timespec *start_time) {
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    int time = (end_time.tv_sec - start_time->tv_sec) * 1000000;
    time += (end_time.tv_nsec - start_time->tv_nsec) / 1000.0;
    return time;
}


void load_image(char *input_file) {
    FILE *file = fopen(input_file, "r");

    char buffer[MAX_LINE_LENGTH + 1];

    fgets(buffer, MAX_LINE_LENGTH, file);
    fgets(buffer, MAX_LINE_LENGTH, file);

    width = atoi(strtok(buffer, WHITESPACE_DELIMITER));
    height = atoi(strtok(NULL, WHITESPACE_DELIMITER));

    fgets(buffer, MAX_LINE_LENGTH, file);

    image = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++){
        image[i] = calloc(width, sizeof(int));
    }

    char *line = NULL;
    size_t len = 0;

    for (int r = 0; r < height; r++) {
        getline(&line, &len, file);

        char *rest = line;
        for (int col = 0; col < width; col++) {
            image[r][col] = atoi(strtok_r(rest, WHITESPACE_DELIMITER, &rest));
        }

    }
    fclose(file);
}


void write_image(char* output_file) {
    FILE* file = fopen(output_file, "w");
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", 255);

    for(int y = 0; y < height; y++) {
        fprintf(file, " ");
        for(int x = 0; x < width; x++) {
            fprintf(file, "%d ", new_image[y][x]);
            if (new_image[y][x] < 100) {
                fprintf(file, " ");
            }
            if (new_image[y][x] < 10) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}


int numbers_handler(int *thread_index) {
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int k = *thread_index;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(image[y][x] < intervals[k]) {
                new_image[y][x] = 255 - image[y][x];
            }
        }
    }

    return calculate_time(&start_time);
}


int block_handler(int *thread_index) {
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int k = *thread_index;
    int x_left = k * ceil((double) width / threads_count);

    int x_right = (k + 1) * ceil((double) width / threads_count) - 1;

    if (x_right < width - 1){
        x_right = width - 1;
    }

    for(int y = 0; y < height; y++) {
        for(int x = x_left; x <= x_right; x++) {
            new_image[y][x]  = 255 - image[y][x];
        }
    }

    return calculate_time(&start_time);
}


int main(int argc, char** argv) {

    threads_count = atoi(argv[1]);
    char* mode = argv[2];
    char* input_file = argv[3];
    char* output_file = argv[4];

    load_image(input_file);

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    new_image = calloc(height, sizeof(int*));
    for(int i = 0; i < height; i++) {
        new_image[i] = calloc(width, sizeof(int));
    }

    pthread_t *threads = calloc(threads_count, sizeof(pthread_t));
    int *args = calloc(threads_count, sizeof(int));

    intervals = calloc(threads_count, sizeof(int));
    for(int i = 0; i < threads_count; i++) {
        intervals[i] = 255 / threads_count + i * (255 / threads_count);
    }

    for(int i = 0; i < threads_count; i++) {
        args[i] = i;
        if(strcmp(mode, "numbers") == 0) {
            pthread_create(&threads[i], NULL, (void*(*)(void *))numbers_handler, args + i);
        }
        else if(strcmp(mode, "block") == 0) {
            pthread_create(&threads[i], NULL, (void*(*)(void *))block_handler, args + i);
        }
    }

    for(int i = 0; i < threads_count; i++) {
        int time;
        pthread_join(threads[i], (void *)&time);
        printf("Thread: %d lasted %d microseconds\n", i, time);
    }

    printf("\nTotal time: %d microseconds\n\n\n", calculate_time(&start_time));

    write_image(output_file);

    free(threads);
    free(args);
    free(intervals);

    for(int i = 0; i < height; i++)
        free(image[i]);
    free(image);

    for(int i = 0; i < height; i++)
        free(new_image[i]);
    free(new_image);

    return 0;
}