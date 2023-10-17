#include <stdio.h>
#include <stdlib.h>

void *mymalloc(size_t size, char *file, int line) {
    printf("Function called from %s, on line number: %d", file, line);
    int dummyAddress = 5;
    return &dummyAddress;
}

void myfree(void *ptr, char *file, int line) {
    return;
}
