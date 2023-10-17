#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALIGNMENT 8 // sizeof(double)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT))
#define HEADER_SIZE sizeof(header_t)

typedef struct memory_header {
    int size;
    bool isFree;
} header_t;

void *mymalloc(size_t size, char *file, int line) {
    printf("Function called from %s, on line number: %d", file, line);
    int dummyAddress = 5;
    return &dummyAddress;
}

void myfree(void *ptr, char *file, int line) {
    return;
}
