#include "library.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char tmp_file[] = "tmp.txt";
char** array;
unsigned int size = 0;

void create_array(unsigned int size_){
    if (array != NULL){
        printf("Array already exists, use remove_array first\n\n");
        return;
    }
    array = calloc(size_, sizeof(char *));
    size = size_;
}

int wc_files(char* files){
    if (array == NULL){
        printf("Array has not been allocated, use create_array first\n\n");
        exit(0);
    }
    char command[10240];

    if (snprintf(command, sizeof(command), "wc %s>> tmp.txt", files) < 0){
        printf("Combined length of file names exceed 10000 characters\n\n");
        exit(0);
    }
    system(command);

    char* block = allocate_block();
    int inserted = 0;
    int index = 0;

    while (index < size){
        if (array[index] == NULL){
            array[index] = block;
            inserted = 1;
            break;
        }
        index++;
    }

    if (inserted == 0){
        printf("Couldn't insert result, array max size has been reached\n\n");
        exit(0);
    }
    return index;
}

char* allocate_block(void){
    char* block;
    FILE* file = fopen(tmp_file, "r");

    if (file == NULL){
        printf("Cannot open file\n\n");
        exit(1);
    }

    if (fseek(file, 0L, SEEK_END) == 0){
        long long file_len = ftell(file);

        if (file_len == -1){
            printf("Couldn't set position indicator to the beginning of the file");
            exit(0);
        }

        block = calloc(file_len, sizeof(char) + 1);

        if (fseek(file, 0L, SEEK_SET) != 0){
            printf("Couldn't read size of the file");
            exit(0);
        }

        size_t new_len = fread(block, sizeof(char), file_len, file);

        if (ferror(file) != 0){
            printf("Error while attempting to read the file");
            exit(0);
        }
        else {
            block[new_len + 1] = '\0';
        }
    }

    fclose(file);
    remove(tmp_file);
    return block;
}

void remove_block(int index){
    if (array == NULL){
        printf("Array has not been created, use create_array first\n\n");
        exit(0);
    }

    else if (size <= index){
        printf("Array size is smaller than requested index of %d\n\n", index);
        exit(0);
    }

    else if (array[index] == NULL){
        printf("There is no block under the requested index of %d\n\n", index);
        exit(0);
    }

    free(array[index]);

    for (int i = index; i < size; ++i) {
        array[i] = array[i + 1];
    }
}

void remove_array() {
    if (array != NULL) {
        for (int i = 0; i < size; ++i) {
            free(array[i]);
        }
        free(array);
        array = NULL;
        size = 0;
    }
}

