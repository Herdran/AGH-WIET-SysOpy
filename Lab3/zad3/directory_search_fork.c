#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>


void func(char *file, char *searched, int depth) {
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
            char* block;
            char path[10240];
            snprintf(path, sizeof(path), "%s/%s", file, pDirent->d_name);
            FILE* file2 = fopen(path, "r");

            if (file2 == NULL){
                printf("Cannot open file\n\n");
                exit(1);
            }

            if (fseek(file2, 0L, SEEK_END) == 0){
                long long file_len = ftell(file2);

                if (file_len == -1){
                    printf("Couldn't set position indicator to the beginning of the file");
                    exit(0);
                }

                block = calloc(file_len, sizeof(char) + 1);

                if (fseek(file2, 0L, SEEK_SET) != 0){
                    printf("Couldn't read size of the file");
                    exit(0);
                }

                size_t new_len = fread(block, sizeof(char), file_len, file2);

                if (ferror(file2) != 0){
                    printf("Error while attempting to read the file\n");
                    printf(" %s\n", file+2);
                    exit(0);
                }
                else {
                    block[new_len + 1] = '\0';
                }
                fclose(file2);
                if (strcmp(searched, block) == 0){
                    char cwd[PATH_MAX];
                    getcwd(cwd, sizeof(cwd));
                    if (file[0] == '.'){
                        printf("This source is at %s/%s/%s\n", cwd, file+2, pDirent->d_name);
                    }
                    else {
                        printf("This source is at %s/%s\n", file, pDirent->d_name);
                    }
                    printf("It was found by process with pid: %d\n", (int) getpid());
                    exit(0);
                }
                free(block);
            }
        }

        if (S_ISDIR(st.st_mode) && depth > 0){
            if (fork() == 0){
                char path[10240];
                snprintf(path, sizeof(path), "%s/%s", file, pDirent->d_name);
                func(path, searched, depth-1);
                exit(0);
            }
        }
    }


    closedir (pDir);

}


int main (int argc, char *argv[]) {
    func(argv[1], argv[2], atoi(argv[3]));

    return 0;
}