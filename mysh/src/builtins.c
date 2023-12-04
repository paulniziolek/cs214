#include "builtins.h"

int cdHomePath(const char* arg);
int cdRelativePath(const char* arg);

// TODO, change char* arg to char** arg and fail when given wrong # of args.
// the above validation is required. 

void execcd(const char* arg) {
    int err = 0;
    
    if (arg == NULL || arg[0] == '\0') {
        perror("cd() fail, no argument specified");
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

void execpwd(int fd) {
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("getcwd() error");
        return;
    }
    write(fd, cwd, strlen(cwd));
    write(fd, "\n", 1);

    free(cwd);
}

void execwhich(int fd, const char* arg) {
    if (arg == NULL || arg[0] == '\0') {
        perror("cd() fail, no argument specified");
        return;
    }

    if (strcmp(arg, "cd") == 0 || strcmp(arg, "which") == 0 || strcmp(arg, "pwd") == 0) {
        const char* builtInMsg = "built-in command: ";
        write(fd, builtInMsg, strlen(builtInMsg));
        write(fd, arg, strlen(arg));
        write(fd, "\n", 1);
        return;
    }
    const char* got = _getExecPath(arg);
    if (got == NULL) {
        perror("not found");
        return;
    }
    write(fd, got, strlen(got));
    write(fd, "\n", 1);
}

const char* _getExecPath(const char* arg) {
    int traversalDirsLen = 3;
    const char* traversalDirs[] = {_BINPATH1, _BINPATH2, _BINPATH3};
    if (arg[0] == '.' || arg[0] == '/') return arg;

    for (int i = 0; i < traversalDirsLen; i++) {
        int newPathLen = strlen(traversalDirs[i]) + strlen(arg) + 2; // +2 for the '/' after binpath and \0.
        char* newPath = (char *)malloc(newPathLen * sizeof(char));
        if (newPath == NULL) {
            perror("malloc() failed for new path in _getExecPath() ");
            return NULL;
        }
        snprintf(newPath, newPathLen, "%s/%s", traversalDirs[i], arg);

        if (access(newPath, F_OK)) {
            // file does not exist
            free(newPath);
            continue;
        } else {
            // file exists
            return newPath;
        }
    }
    return NULL;
}