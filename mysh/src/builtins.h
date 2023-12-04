#ifndef BUILTINS_H
#define BUILTINS_H

#define _ENV_VAR_HOME "HOME"
#define _BINPATH1 "/usr/local/bin"
#define _BINPATH2 "/usr/bin"
#define _BINPATH3 "/bin"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"

void cd(const char* arg);

void pwd(int fd);

void which(int fd, const char* arg);

const char* _getExecPath(const char* arg);

#endif