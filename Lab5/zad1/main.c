#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_ARGS 20
#define MAX_LINES 10


char** get_program_commands(char* line){
    char** commands = (char**)calloc(MAX_ARGS, sizeof(char*));
    char* line_copy = calloc(strlen(line), sizeof(char));
    strcpy(line_copy, line);
    char* command = strtok(line_copy, "=");
    int ctr = 0;
    while ((command = strtok(NULL, "|"))){
        command[strlen(command)-1] = '\0';
        commands[ctr++] = command + 1;
    }
    return commands;
}


char** get_program_args(char* command){
    char** args = (char**)calloc(256, sizeof(char*));
    char* arg = strtok(command, " ");
    args[0] = arg;
    int ctr = 1;
    while ((arg = strtok(NULL, " "))){
        args[ctr++] = arg;
    }
    return args;
}


int* get_lines_to_execute(char* line, int line_ctr){
    char** lines = (char**)calloc(MAX_ARGS, sizeof(char*));
    char* arg = strtok(line, " ");

    int ctr = 0;
    lines[ctr++] = arg;
    while ((arg = strtok(NULL, " "))){
        lines[ctr++] = arg;
    }

    static int lines_num[MAX_ARGS];
    int i = 0;
    while(lines[i]) {
        lines_num[i] = lines[i][8] - '0';
        i++;
    }
    lines_num[i] = -1;

    return lines_num;
}


void parse_execute(FILE* file){
    char** lines = (char**)calloc(MAX_LINES, sizeof(char*));
    char** commands;
    char** args;
    char* line = (char*)calloc(256, sizeof(char));
    int* lines_num;
    int line_ctr = 0;
    int lines_number;

    while(fgets(line, 256 * sizeof(char), file)) {
        if (strstr(line, "=")) {
            char *line_copy = (char *) calloc(256, sizeof(char));
            strcpy(line_copy, line);
            lines[line_ctr] = line_copy;
            line_ctr++;
        }
        else {
            lines_num = get_lines_to_execute(line, line_ctr);

            lines_number = 0;
            for (int i = 0; lines_num[i] != -1; i++) {
                lines_number++;
            }

            int pipe_in[2];
            int pipe_out[2];

            pipe(pipe_out);

            for (int i = 0; i < lines_number; i++) {
                commands = get_program_commands(lines[lines_num[i]]);
                int m = 0;
                while (commands[m]) {
                    m++;
                }

                for (int j = 0; j < m; j++) {
                    if(fork() == 0) {
                        if (j == 0 && i == 0) {
                            close(pipe_out[0]);
                            dup2(pipe_out[1], STDOUT_FILENO);
                        }
                        else if (j == m - 1 && i == lines_number - 1) {
                            close(pipe_out[0]);
                            close(pipe_out[1]);
                            close(pipe_in[1]);
                            dup2(pipe_in[0], STDIN_FILENO);
                        }
                        else {
                            close(pipe_in[1]);
                            close(pipe_out[0]);
                            dup2(pipe_in[0], STDIN_FILENO);
                            dup2(pipe_out[1], STDOUT_FILENO);
                        }
                        args = get_program_args(commands[j]);

                        if (execvp(args[0], args) == -1) {
                            printf("ERROR in exec\n");
                            exit(1);
                        }

                    }
                    else {
                        close(pipe_in[1]);
                        pipe_in[0] = pipe_out[0];
                        pipe_in[1] = pipe_out[1];

                    }
                    if (pipe(pipe_out) != 0) {
                        printf("Error while moving pipe\n");
                        exit(1);
                    }
                    wait(0);
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    FILE* commands = fopen(argv[1], "r");

    if (commands == NULL){
        printf("Cannot open file\n");
        exit(1);
    }

    parse_execute(commands);
    fclose(commands);
}