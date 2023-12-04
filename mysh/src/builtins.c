#include "builtins.h"

int cdHomePath(const char* arg);
int cdRelativePath(const char* arg);

void cd(const char* arg) {
    int err = 0;
    
    if (arg == NULL || arg[0] == '\0') {
        perror("cd() fail, no arguments specified");
        return;
    }

    switch (arg[0]) {
    case '/': 
        err = chdir(arg);
        break;
    case '~':
        err = cdHomePath(arg);
        break;
    default:
        err = cdRelativePath(arg);
    }

    if (err != 0) {
        perror("cd() fail");
    }
}

int cdHomePath(const char* arg) {
    int err = 0;

    const char* homeDir = getenv(_ENV_VAR_HOME);
    if (homeDir == NULL) {
        perror("HOME env var not set ");
        return err;
    }

    int newPathLen = strlen(homeDir) + strlen(arg) + 2; // +2 for the '/' after cwd and \0.
    char* newPath = (char *)malloc(newPathLen * sizeof(char));
    if (newPath == NULL) {
        perror("malloc() failed for new path in cd() ");
        return err;
    }
    snprintf(newPath, newPathLen, "%s%s", homeDir, arg + 1);

    err = chdir(newPath);
    free(newPath);
    return err;
}

int cdRelativePath(const char* arg) {
    int err = 0;
    const char* cwd = getcwd(NULL, 0);

    int newPathLen = strlen(cwd) + strlen(arg) + 2; // +2 for the '/' after cwd and \0.
    char* newPath = (char *)malloc(newPathLen * sizeof(char));
    if (newPath == NULL) {
        perror("malloc() failed for new path in cd() ");
        return err;
    }
    snprintf(newPath, newPathLen, "%s/%s", cwd, arg);

    err = chdir(newPath);
    free(newPath);
    return err;
}

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
