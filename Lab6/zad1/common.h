#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/msg.h>

#define MAX_CLIENTS 5
#define MAX_MESSAGE_SIZE 128
#define PATH_TO_GENERATE_KEY getenv("HOME")
#define SERVER_KEY_ID 1

typedef enum mtype {
    INIT = 1, LIST = 2, TO_ONE = 3, TO_ALL = 4, STOP = 5
} mtype;

typedef struct msgbuf {
    long mtype;
    char mtext[MAX_MESSAGE_SIZE];
    key_t queue_key;
    int client_id;
    int other_client_id;
    struct tm tm;
} msgbuf;

const int MSG_SIZE = sizeof(msgbuf);