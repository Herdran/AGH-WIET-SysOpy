#include "common.h"

key_t clients_queues[MAX_CLIENTS];
int clients_available[MAX_CLIENTS];
int first_free_id = 0;
int server_queue;


void log_msg(msgbuf *msg) {
    struct tm tm = msg->tm;

    FILE* log_fd = fopen("server_logs.txt", "a");

    switch(msg->mtype) {
        case INIT:
            if (msg->client.client_id == -1){
                fprintf(log_fd, "INIT received but client limit reached\n");
            }
            else{
                fprintf(log_fd, "INIT of client with id %d\n", msg->client.client_id);
            }
            break;
        case LIST:
            fprintf(log_fd, "LIST requested by client with id %d\n", msg->client.client_id);
            break;
        case TO_ONE:
            fprintf(log_fd, "2ALL from client with id %d\n", msg->client.client_id);
            fprintf(log_fd, "Message sent: %s\n" , msg->mtext);
            break;
        case TO_ALL:
            fprintf(log_fd, "2ONE from client with id %d to client with id %d\n", msg->client.client_id, msg->client.to_one_client_id);
            fprintf(log_fd, "Message sent: %s\n" , msg->mtext);
            break;
        case STOP:
            fprintf(log_fd, "STOP of client with id %d\n", msg->client.client_id);
            break;
    }

    fprintf(log_fd, "Time: %d-%02d-%02d %02d:%02d:%02d\n\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(log_fd);
}


void init_handler(msgbuf* msg) {
    while (first_free_id < MAX_CLIENTS - 1 && clients_queues[first_free_id] != -1){
        first_free_id++;
    }
    if (first_free_id == MAX_CLIENTS - 1 && clients_queues[first_free_id] != -1){
        printf("INIT received but client limit reached\n");
        msg->client.client_id = -1;
        int client_queue_id = msgget(msg->client.queue_key, 0);
        msgsnd(client_queue_id, msg, MSG_SIZE, 0);
        log_msg(msg);
    }
    else{
        printf("INIT received\n");
        msg->client.client_id = first_free_id;

        int client_queue_id = msgget(msg->client.queue_key, 0);

        clients_queues[first_free_id] = msg->client.queue_key;
        clients_available[first_free_id] = 1;
        if (first_free_id < MAX_CLIENTS - 1){
            first_free_id++;
        }
        msgsnd(client_queue_id, msg, MSG_SIZE, 0);
        log_msg(msg);
    }
}

void list_handler(int client_id) {
    msgbuf* msg = malloc(sizeof(msgbuf));
    printf("LIST received\n");
    strcpy(msg->mtext, "");

    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients_queues[i] != -1) {
            sprintf(msg->mtext + strlen(msg->mtext), "ID %d, client %s\n", i , clients_available[i] ? "available" : "not available");
        }
    }

    int client_queue_id = msgget(clients_queues[client_id], 0);

    msg->mtype = LIST;
    msgsnd(client_queue_id, msg, MSG_SIZE, 0);
}


void to_one_handler(msgbuf* msg) {
    printf("2ONE received\n");
    int other_client_queue_id = msgget(clients_queues[msg->client.to_one_client_id], 0);
    msgsnd(other_client_queue_id, msg, MSG_SIZE, 0);
}


void to_all_handler(msgbuf* msg) {
    printf("2ALL received\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (i != msg->client.client_id && clients_queues[i] != -1){
            int other_client_queue_id = msgget(clients_queues[i], 0);
            msgsnd(other_client_queue_id, msg, MSG_SIZE, 0);
        }
    }
}


void stop_handler(int client_id) {
    printf("STOP received\n");
    clients_queues[client_id] = -1;
    clients_available[client_id] = 0;
    first_free_id = client_id;
}


void quit_handler() {
    msgbuf* msg = malloc(sizeof(msgbuf));
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients_queues[i] != -1) {
            msg->mtype = STOP;
            int client_queue_id = msgget(clients_queues[i], 0);
            msgsnd(client_queue_id, msg, MSG_SIZE, 0);
            msgrcv(server_queue, msg, MSG_SIZE, STOP, 0);
        }
    }
    msgctl(server_queue, IPC_RMID, NULL);
    exit(0);
}

int main() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        clients_queues[i] = -1;
    }
    key_t queue_key = ftok(PATH_TO_GENERATE_KEY, SERVER_KEY_ID);
    server_queue = msgget(queue_key, IPC_CREAT | 0666);

    printf("Server queue key: %d\n", queue_key);
    printf("Server queue ID: %d\n", server_queue);

    signal(SIGINT, quit_handler);

    msgbuf* msg = malloc(sizeof(msgbuf));
    while(1) {
        msgrcv(server_queue, msg, MSG_SIZE, -6, 0);

        switch(msg->mtype) {
            case INIT:
                init_handler(msg);
                break;
            case LIST:
                list_handler(msg->client.client_id);
                log_msg(msg);
                break;
            case TO_ONE:
                to_one_handler(msg);
                log_msg(msg);
                break;
            case TO_ALL:
                to_all_handler(msg);
                log_msg(msg);
                break;
            case STOP:
                stop_handler(msg->client.client_id);
                log_msg(msg);
                break;
            default:
                printf("Message type doesn't exist\n");
        }
    }
}