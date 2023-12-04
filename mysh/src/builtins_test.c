#include "builtins.h"

int main() {
    int fd = STDOUT_FILENO;
    cd("../../../");
    pwd(fd);
    cd("~/Downloads");
    cd("path/that/doesnt/exist");
    cd("");
    pwd(fd);

}
