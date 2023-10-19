#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "helper_free.h"
#include "helper_malloc.h"

// User-facing macros
#define malloc(s) mymalloc(s, __FILE__, __LINE__)
#define free(p) myfree(p, __FILE__, __LINE__)

// Internal macros
// 8 bytes
#define ALIGNMENT (int) sizeof(double)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define HEADER_SIZE sizeof(header_t)
// size of header in blocks (size of block = size of alignment)
#define HEADER_SIZE_IN_BLOCKS ((HEADER_SIZE / ALIGNMENT) + (HEADER_SIZE % ALIGNMENT != 0))
#define HEAP_SIZE_IN_BLOCKS 512
// 4096 bytes
#define HEAP_SIZE (HEAP_SIZE_IN_BLOCKS * ALIGNMENT)
// one past the end of the heap
#define END (header_t *) (heap + HEAP_SIZE_IN_BLOCKS)

// User-facing functions
void *mymalloc_wrapper(size_t size, char *file, int line);
void myfree_wrapper(void *ptr, char *file, int line);

// Memory
static double heap[HEAP_SIZE_IN_BLOCKS]; 

typedef struct memory_header {
    int size; 
    bool isFree;
    struct memory_header *next;
} header_t;

// Internal functions
header_t* nextHeader(header_t *header);
header_t* cut(header_t *header, int size);
bool canCoalesce(header_t *header);

// Debugging Functions
void printMemory();

#endif
