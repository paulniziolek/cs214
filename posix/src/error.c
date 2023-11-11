#include "error.h"

void writeErr(const char *format, ...) {
    char errorMessage[ERR_MSG_SIZE];
    va_list args;
    
    va_start(args, format);
    int needed = vsnprintf(errorMessage, sizeof(errorMessage), format, args);
    va_end(args);

    if (needed >= sizeof(errorMessage)) {
        const char *truncationMsg = "Error: The error message was truncated\n";
        write(STDERR_FD, truncationMsg, strlen(truncationMsg));
    } else {
        write(STDERR_FD, errorMessage, needed);
    }
}
