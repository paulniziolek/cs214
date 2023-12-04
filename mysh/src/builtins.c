#include "builtins.h"

void cd(const char* arg) {}

void pwd(int fd) {
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("getcwd() error");
        return;
    }
    write(fd, cwd, strlen(cwd));
    write(fd, "\n", 1);

    free(cwd);
}