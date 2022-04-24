#include "common.h"

mqd_t queue_desc;
mqd_t server_queue_desc;
int client_id;
char queue_name[NAME_LEN];

int init(){
    msgbuf msg;
    time_t t = time(NULL);
    msg.tm = *localtime(&t);
    msg.mtype = INIT;
    strcpy(msg.mtext, queue_name);
    mq_send(server_queue_desc, (char *) &msg, MSG_SIZE, 0);
    mq_receive(queue_desc, (char *) &msg, MSG_SIZE, NULL);
    client_id = msg.client_id;
    if (client_id == -1){
        printf("Max amount of clients has been reached, exiting\n");
        exit(0);
    }
    return client_id;
}


void list_command() {
    msgbuf msg;
    time_t t = time(NULL);
    msg.tm = *localtime(&t);
    msg.mtype = LIST;
    msg.client_id = client_id;
    mq_send(server_queue_desc, (char *) &msg, MSG_SIZE, 0);
    mq_receive(queue_desc, (char *) &msg, MSG_SIZE, NULL);
    printf("%s\n", msg.mtext);
}


void to_one_command(int other_client_id, char* message) {
    msgbuf msg;
    time_t t = time(NULL);
    msg.tm = *localtime(&t);
    msg.mtype = TO_ONE;
    strcpy(msg.mtext, message);
    msg.client_id = client_id;
    msg.other_client_id = other_client_id;
    mq_send(server_queue_desc, (char *) &msg, MSG_SIZE, 0);
}


void to_all_command(char* message) {
    msgbuf msg;
    time_t t = time(NULL);
    msg.tm = *localtime(&t);
    msg.mtype = TO_ALL;
    strcpy(msg.mtext, message);
    msg.client_id = client_id;
    mq_send(server_queue_desc, (char *) &msg, MSG_SIZE, 0);
}


void stop() {
    msgbuf msg;
    time_t t = time(NULL);
    msg.tm = *localtime(&t);
    msg.mtype = STOP;
    msg.client_id = client_id;
    mq_send(server_queue_desc, (char *) &msg, MSG_SIZE, 0);
    mq_close(server_queue_desc);
    exit(0);
}


void check_server_message() {
    msgbuf msg;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);
    tm.tv_sec += 0.2;
    while(mq_timedreceive(queue_desc, (char *) &msg, MSG_SIZE, NULL, &tm) != -1){
        if(msg.mtype == STOP) {
            printf("STOP from server, quitting...\n");
            stop();
        }
        else{
            printf("%s\n", msg.mtext);
        }
    }
}


void generate_name() {
    queue_name[0] = '/';
    for(int i = 1; i < NAME_LEN; i++) queue_name[i] = random_char();
}


int main() {
    srand(time(NULL));
    generate_name();
    queue_desc = create_queue(queue_name);
    server_queue_desc = mq_open(SERVER_QUEUE_NAME, O_RDWR);
    client_id = init();

    printf("Queue name %s\n", queue_name);
    printf("ID received: %d\n", client_id);

    signal(SIGINT, stop);

    char *command = NULL;
    size_t len = 0;
    ssize_t read = 0;

    while (1) {
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