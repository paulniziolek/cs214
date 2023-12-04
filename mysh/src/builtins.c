#include "builtins.h"

void cd(const char* arg) {}

void pwd(int fd) {
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("getcwd() error");
        return;
    }
    dprintf(fd, "%s\n", cwd);
    free(cwd);
}