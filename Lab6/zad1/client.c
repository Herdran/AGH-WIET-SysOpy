#include "common.h"

key_t queue_key;
int queue_id;
int server_queue_id;
int client_id;


int init(){
    msgbuf* msg = malloc(sizeof(msgbuf));
    time_t t = time(NULL);
    msg->tm = *localtime(&t);
    msg->mtype = INIT;
    msg->client.queue_key = queue_key;
    msgsnd(server_queue_id, msg, MSG_SIZE, 0);

    msgbuf* msg_rcv = malloc(sizeof(msgbuf));
    msgrcv(queue_id, msg_rcv, MSG_SIZE, 0, 0);

    int client_id = msg_rcv->client.client_id;
    if (client_id == -1){
        printf("Max amount of clients has been reached, exiting\n");
        exit(0);
    }
    return client_id;
}


void list_command() {
    msgbuf* msg = malloc(sizeof(msgbuf));
    msgbuf* msg_rcv = malloc(sizeof(msgbuf));
    time_t t = time(NULL);
    msg->tm = *localtime(&t);
    msg->mtype = LIST;
    msg->client.client_id = client_id;

    msgsnd(server_queue_id, msg, MSG_SIZE, 0);
    msgrcv(queue_id, msg_rcv, MSG_SIZE, 0, 0);
    printf("%s\n", msg_rcv->mtext);
}


void to_one_command(int other_client_id, char* message) {
    msgbuf* msg = malloc(sizeof(msgbuf));
    time_t t = time(NULL);
    msg->tm = *localtime(&t);
    msg->mtype = TO_ONE;
    strcpy(msg->mtext, message);
    msg->client.client_id = client_id;
    msg->client.to_one_client_id = other_client_id;
    msgsnd(server_queue_id, msg, MSG_SIZE, 0);
}


void to_all_command(char* message) {
    msgbuf* msg = malloc(sizeof(msgbuf));
    time_t t = time(NULL);
    msg->tm = *localtime(&t);
    msg->mtype = TO_ALL;
    strcpy(msg->mtext, message);
    msg->client.client_id = client_id;
    msgsnd(server_queue_id, msg, MSG_SIZE, 0);
}


void stop() {
    msgbuf* msg = malloc(sizeof(msgbuf));
    time_t t = time(NULL);
    msg->tm = *localtime(&t);
    msg->mtype = STOP;
    msg->client.client_id = client_id;

    msgsnd(server_queue_id, msg, MSG_SIZE, 0);
    msgctl(queue_id, IPC_RMID, NULL);

    exit(0);
}


void check_server_message() {
    msgbuf* msg_rcv = malloc(sizeof(msgbuf));

    while(msgrcv(queue_id, msg_rcv, MSG_SIZE, 0, IPC_NOWAIT) >= 0) {
        if(msg_rcv->mtype == STOP) {
            printf("STOP from server, quitting...\n");
            stop();
        }
        else{
            struct tm tm = msg_rcv->tm;
            printf("Message from user with id %d sent on %d-%02d-%02d %02d:%02d:%02d:\n%s\n",
                   msg_rcv->client.client_id, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, msg_rcv->mtext);
        }
    }
}


int main() {
    srand(time(NULL));

    queue_key = ftok(PATH_TO_GENERATE_KEY, rand() % 255 + 1);

    queue_id = msgget(queue_key, IPC_CREAT | 0666);

    key_t server_key = ftok(PATH_TO_GENERATE_KEY, SERVER_KEY_ID);
    server_queue_id = msgget(server_key, 0);

    client_id = init();

    printf("Queue key: %d\n", queue_key);
    printf("Queue ID: %d\n", queue_id);
    printf("Server queue ID: %d\n", server_queue_id);
    printf("ID received: %d\n", client_id);

    signal(SIGINT, stop);

    char* command = NULL;
    size_t len = 0;
    ssize_t read = 0;
    while(1) {
        printf("Enter the command: ");
        read = getline(&command, &len, stdin);
        command[read - 1] = '\0';

        check_server_message();

        if(strcmp(command, "") == 0) {
            continue;
        }

        char* tok = strtok(command, " ");

        if(strcmp(tok, "LIST") == 0) {
            printf("LIST command\n");
            list_command();
        }
        else if(strcmp(tok, "2ALL") == 0){
            printf("2ALL command\n");
            tok = strtok(NULL, " ");
            char* message = tok;
            to_all_command(message);
        }
        else if(strcmp(tok, "2ONE") == 0){
            printf("2ONE command\n");
            tok = strtok(NULL, " ");
            int id_to_send = atoi(tok);
            tok = strtok(NULL, " ");
            char* message = tok;
            to_one_command(id_to_send, message);
        }
        else if(strcmp(tok, "STOP") == 0){
            printf("STOP command\n");
            stop();
        }
        else{
            printf("Bad format of the command\n");
        }
    }
}