#include "builtins.h"

int main() {
    int fd = STDOUT_FILENO;
    execcd("../../.././");
    execpwd(fd);
    execcd("~/Downloads");
    execcd("path/that/doesnt/exist");
    execcd("");
    execcd(".");
    execpwd(fd);

    execwhich(fd, "which");
    execwhich(fd, "cd");
    execwhich(fd, "pwd");

    execwhich(fd, "ls");
}
