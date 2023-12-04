#include "builtins.h"

int main() {
    int fd = STDOUT_FILENO;
    cd("../../.././");
    pwd(fd);
    cd("~/Downloads");
    cd("path/that/doesnt/exist");
    cd("");
    cd(".");
    pwd(fd);

    which(fd, "which");
    which(fd, "cd");
    which(fd, "pwd");

    which(fd, "ls");
}
