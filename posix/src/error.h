#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STDERR_FD 2
#define ERR_MSG_SIZE 512

// Will write a formatted error message
void writeError(const char *format, ...);

#endif
