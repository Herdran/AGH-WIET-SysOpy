#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAX_CLIENTS 5
#define MAX_MESSAGE_SIZE 128
#define PATH_TO_GENERATE_KEY getenv("HOME")

typedef enum mtype {
    INIT = 1, LIST = 2, TO_ONE = 3, TO_ALL = 4, STOP = 5
} mtype;

typedef struct client {
    key_t queue_key;
    int client_id;
    int to_one_client_id;
} client;

typedef struct msgbuf {
    long mtype;
    char mtext[MAX_MESSAGE_SIZE];
    struct tm tm;
    client client;
} msgbuf;

const int SERVER_KEY_ID = 1;
const int MSG_SIZE = sizeof(msgbuf) - sizeof(long);