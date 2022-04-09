#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>
#include <unistd.h>
#include <limits.h>

int file_count = 0;
int dir_count = 0;
int char_dev_count = 0;
int block_dev_count = 0;
int fifo_count = 0;
int slink_count = 0;
int sock_count = 0;


int func(char *pathname, struct stat *st, int type, struct FTW *pfwt) {
    if (type == FTW_F){
        if (S_ISREG(st->st_mode)) {
            file_count++;
            printf("file\n");
        }
        else if (S_ISCHR(st->st_mode)) {
            char_dev_count++;
            printf("char dev\n");
        }
        else if (S_ISBLK(st->st_mode)) {
            block_dev_count++;
            printf("block dev\n");
        }
        else if (S_ISFIFO(st->st_mode)) {
            fifo_count++;
            printf("fifo\n");
        }
        else if (S_ISLNK(st->st_mode)) {
            slink_count++;
            printf("slink\n");
        }
        else if (S_ISSOCK(st->st_mode)) {
            sock_count++;
            printf("sock\n");
        }
    }
    else {
        dir_count++;
        printf("dir\n");
    }


    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    if (pathname[0] == '.'){
        printf("This source is at %s/%s\n", cwd, pathname+2);
    }
    else {
        printf("This source is at %s\n", pathname);
    }
    printf("%ld hard links\n", st->st_nlink);
    printf("%ld bytes\n", st->st_size);
    printf("Last accessed on %s", ctime(&st->st_atime));
    printf("Last modified on %s", ctime(&st->st_mtime));
    printf("\n");

    return 0;
}


int main (int argc, char *argv[]) {
    void *fn = func;
    nftw(argv[1], fn, 5, FTW_PHYS);

    printf("regular files = %d\n", file_count);
    printf("directories= %d\n", dir_count);
    printf("char special = %d\n", block_dev_count);
    printf("block special = %d\n", char_dev_count);
    printf("FIFO = %d\n", fifo_count);
    printf("symbolic links = %d\n", slink_count);
    printf("sockets = %d", sock_count);
    return 0;
}