#include "common.h"

char* clients_queues[MAX_CLIENTS];
int first_free_id = 0;
mqd_t server_queue;


void log_msg(msgbuf msg) {
    struct tm tm = msg.tm;

    FILE* log_fd = fopen("server_logs.txt", "a");

    switch(msg.mtype) {
        case INIT:
            if (msg.client_id == -1){
                fprintf(log_fd, "INIT received but client limit reached\n");
            }
            else{
                fprintf(log_fd, "INIT of client with ID %d\n", msg.client_id);
            }
            break;
        case LIST:
            fprintf(log_fd, "LIST requested by client with ID %d\n", msg.client_id);
            break;
        case TO_ONE:
            fprintf(log_fd, "2ONE from client with ID %d to client with ID %d\n", msg.client_id, msg.other_client_id);
            fprintf(log_fd, "Message sent: %s\n" , msg.mtext);
            break;
        case TO_ALL:
            fprintf(log_fd, "2ALL from client with ID %d\n", msg.client_id);
            fprintf(log_fd, "Message sent: %s\n" , msg.mtext);
            break;
        case STOP:
            fprintf(log_fd, "STOP of client with ID %d\n", msg.client_id);
            break;
    }

    fprintf(log_fd, "Time: %d-%02d-%02d %02d:%02d:%02d\n\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(log_fd);
}


void init_handler(msgbuf msg) {
    while (first_free_id < MAX_CLIENTS - 1 && clients_queues[first_free_id] != NULL){
        first_free_id++;
    }
    if (first_free_id == MAX_CLIENTS - 1 && clients_queues[first_free_id] != NULL){
        printf("INIT received but client limit reached\n");
        msg.client_id = -1;
    }
    else{
        printf("INIT received\n");
        msg.client_id = first_free_id;
        clients_queues[first_free_id] = (char*)calloc(NAME_LEN, sizeof(char));
        strcpy(clients_queues[first_free_id], msg.mtext);
        if (first_free_id < MAX_CLIENTS - 1){
            first_free_id++;
        }
    }
    mqd_t client_queue_desc = mq_open(msg.mtext, O_RDWR);
    mq_send(client_queue_desc, (char *) &msg, MSG_SIZE, 0);
    mq_close(client_queue_desc);
    log_msg(msg);
}


void list_handler(int client_id) {
    msgbuf msg;
    printf("LIST received\n");
    strcpy(msg.mtext, "");

    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients_queues[i] != NULL) {
            sprintf(msg.mtext + strlen(msg.mtext), "ID %d, client active\n", i);
        }
    }
    mqd_t client_queue_desc = mq_open(clients_queues[client_id], O_RDWR);
    mq_send(client_queue_desc, (char *) &msg, MSG_SIZE, 0);
    mq_close(client_queue_desc);
}


void to_one_handler(msgbuf msg) {
    printf("2ONE received\n");
    mqd_t other_client_queue_desc = mq_open(clients_queues[msg.other_client_id], O_RDWR);
    mq_send(other_client_queue_desc, (char *) &msg, MSG_SIZE, 0);
    mq_close(other_client_queue_desc);
}


void to_all_handler(msgbuf msg) {
    printf("2ALL received\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (i != msg.client_id && clients_queues[i] != NULL){
            mqd_t other_client_queue_desc = mq_open(clients_queues[i], O_RDWR);
            mq_send(other_client_queue_desc, (char *) &msg, MSG_SIZE, 0);
            mq_close(other_client_queue_desc);
        }
    }
}


void stop_handler(int client_id) {
    printf("STOP received\n");
    clients_queues[client_id] = NULL;
    if (client_id < first_free_id){
        first_free_id = client_id;
    }
}


void quit_handler() {
    msgbuf msg;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients_queues[i] != NULL) {
            msg.mtype = STOP;
            mqd_t other_client_queue_desc = mq_open(clients_queues[i], O_RDWR);
            mq_send(other_client_queue_desc, (char *) &msg, MSG_SIZE, 0);
            mq_receive(server_queue, (char *) &msg, MSG_SIZE, NULL);
            mq_close(other_client_queue_desc);
        }
    }
    mq_close(server_queue);
    exit(0);
}

int main() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        clients_queues[i] = NULL;
    }
    mq_unlink(SERVER_QUEUE_NAME);
    server_queue = create_queue(SERVER_QUEUE_NAME);
    signal(SIGINT, quit_handler);
    msgbuf msg;
    printf("SERVER START\n");

    while(1) {
        mq_receive(server_queue, (char *) &msg, MSG_SIZE, NULL);

        switch(msg.mtype) {
            case INIT:
                init_handler(msg);
                break;
            case LIST:
                list_handler(msg.client_id);
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
                stop_handler(msg.client_id);
                log_msg(msg);
                break;
            default:
                printf("Message type doesn't exist\n");
        }
    }
}