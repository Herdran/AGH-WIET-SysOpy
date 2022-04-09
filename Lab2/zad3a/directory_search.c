#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int file_count = 0;
int dir_count = 0;
int char_dev_count = 0;
int block_dev_count = 0;
int fifo_count = 0;
int slink_count = 0;
int sock_count = 0;

void func(char *file) {
    struct dirent *pDirent;
    DIR *pDir;
    pDir = opendir (file);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", file);
        exit(0);
    }


    while ((pDirent = readdir(pDir)) != NULL) {
        struct stat st;

        if(strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
            continue;
        }

        if (fstatat(dirfd(pDir), pDirent->d_name, &st, 0) < 0) {
            perror(pDirent->d_name);
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            file_count++;
            printf("file");
        }
        else if (S_ISDIR(st.st_mode)) {
            dir_count++;
            printf("dir");
        }
        else if (S_ISCHR(st.st_mode)) {
            char_dev_count++;
            printf("char dev");
        }
        else if (S_ISBLK(st.st_mode)) {
            block_dev_count++;
            printf("block dev");
        }
        else if (S_ISFIFO(st.st_mode)) {
            fifo_count++;
            printf("fifo");
        }
        else if (S_ISLNK(st.st_mode)) {
            slink_count++;
            printf("slink");
        }
        else if (S_ISSOCK(st.st_mode)) {
            sock_count++;
            printf("sock");
        }

//        printf("% s\n", pDirent->d_name);
        printf("\n");

        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));

        if (file[0] == '.'){
            printf("This source is at %s/%s/%s\n", cwd, file+2, pDirent->d_name);
        }
        else {
            printf("This source is at %s/%s\n", file, pDirent->d_name);
        }
        printf("%ld hard links\n", st.st_nlink);
        printf("%ld bytes\n", st.st_size);
        printf("Last accessed on %s", ctime(&st.st_atime));
        printf("Last modified on %s", ctime(&st.st_mtime));
        printf("\n");
    }

    closedir (pDir);
    pDir = opendir (file);

    while ((pDirent = readdir(pDir)) != NULL) {
        struct stat st;
        if (fstatat(dirfd(pDir), pDirent->d_name, &st, 0) < 0) {
            perror(pDirent->d_name);
            continue;
        }
        if(strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)){
            char path[10240];
            snprintf(path, sizeof(path), "%s/%s", file, pDirent->d_name);
            func(path);
        }
    }
    closedir (pDir);
}


int main (int argc, char *argv[]) {
    func(argv[1]);
    printf("regular files = %d\n", file_count);
    printf("directories= %d\n", dir_count);
    printf("char special = %d\n", block_dev_count);
    printf("block special = %d\n", char_dev_count);
    printf("FIFO = %d\n", fifo_count);
    printf("symbolic links = %d\n", slink_count);
    printf("sockets = %d", sock_count);

    return 0;
}