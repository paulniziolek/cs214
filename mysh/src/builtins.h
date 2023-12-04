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

// cd() will change directory to the specified argument directory.
// cd accepts:
//      absolute paths, i.e: "/absolute/path"
//      relative paths, i.e: "relative/path"
//      home paths, i.e: "~/Documents"
// if the directory does not exist, cd will return an error msg to stderr.
void cd(const char* arg);

// pwd() will write the current working directory to the specified fd number.
void pwd(int fd);

// which() will write the specified argument executable to the specified fd number.
// which will search these directories in order:
// /usr/local/bin -> /usr/bin -> /bin
// if no executable exists, then which will return an error msg to stderr.
void which(int fd, const char* arg);

// _getExecPath() is an internal function to find a bare named executable/program.
// _getExecPath will search these directories in order:
// /usr/local/bin -> /usr/bin -> /bin
const char* _getExecPath(const char* arg);

#endif