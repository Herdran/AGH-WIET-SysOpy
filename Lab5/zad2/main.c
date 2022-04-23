#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256

void print_output(FILE* file){
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file) != NULL){
        printf("%s\n", line);
    }
}

void print_ordered_by(char* mode){
    FILE* file;
    char* command;

    if (strcmp(mode, "date") == 0) {
        command = "echo | mail -f | tail +2 | head -n -1 | tac";
    }
    else if (strcmp(mode, "sender") == 0){
        command = "echo | mail -f | tail +2 | head -n -1 | sort -k 2";
    }
    printf("%s\n", command);
    file = popen(command, "r");
    printf("Sorted emails:\n");
    print_output(file);
}

void send_email(char* address, char* object, char* content){
    char command[MAX_SIZE];

    snprintf(command, sizeof(command), "echo %s | mail -s %s %s", content, object, address);
    printf("%s\n", command);
    popen(command, "r");

    printf("\n----- Email has been sent! -----\n");
    printf("TO:      %s\nOBJECT:  %s\nCONTENT: %s\n", address, object, content);

}

int main(int argc, char* argv[]){
    if (argc != 2 && argc != 4){
        printf("Wrong number of arguments!");
        exit(1);
    }

    if (argc == 2){
        printf("Print emails ordered by %s\n", argv[1]);
        print_ordered_by(argv[1]);
    }
    else {
        printf("Send email: \n");
        send_email(argv[1], argv[2], argv[3]);
    }
    return 0;
}