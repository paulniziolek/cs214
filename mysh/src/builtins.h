#ifndef BUILTINS_H
#define BUILTINS_H

#define _ENV_VAR_HOME "HOME"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unistd.h"

void cd(const char* arg);

void pwd(int fd);

void which(int fd, const char* arg);

#endif