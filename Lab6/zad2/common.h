#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>

#define MAX_CLIENTS 5
#define MAX_MESSAGE_SIZE 128
#define NAME_LEN 3
#define SERVER_QUEUE_NAME "/SERVER"

typedef enum mtype {
    INIT = 1, LIST = 2, TO_ONE = 3, TO_ALL = 4, STOP = 5
} mtype;

typedef struct msgbuf{
    long mtype;
    char mtext[MAX_MESSAGE_SIZE];
    int client_id;
    int other_client_id;
    struct tm tm;
} msgbuf;

const int MSG_SIZE = sizeof(msgbuf);

mqd_t create_queue(const char* name) {
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_CLIENTS;
    attr.mq_msgsize = MSG_SIZE;
    return mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
}

char random_char() {
    return rand() % ('Z' - 'A' + 1) + 'A';
}