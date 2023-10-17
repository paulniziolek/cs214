#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALIGNMENT 8 // sizeof(double)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT))
#define HEADER_SIZE sizeof(header_t)
#define HEAP_SIZE 512

static double heap[HEAP_SIZE]; 

typedef struct memory_header {
    int size; 
    bool isFree;
} header_t;

header_t* nextHeader(header_t *header){
    if((header->size + HEADER_SIZE) >= HEAP_SIZE) return NULL;
    return (header_t *) &heap[header->size + HEADER_SIZE];
}

void *mymalloc(size_t size, char *file, int line) {
    //first checking if this is the first time malloc was called
    //i.e there is no header struct at the beginning of the heaps
    if (heap[0] == 0) {
        header_t *header = (header_t *) &heap[0];
        header->size = HEAP_SIZE - HEADER_SIZE;
        header->isFree = true;
    }

    //tricky two ptr approach
    header_t *header = (header_t *) &heap[0];
    while (header != NULL) {
        if (header->isFree) {
            if( header->size >= size){
                header->isFree = false;
                return (void *) &header[1];
            } else if(nextHeader(header) != NULL && nextHeader(header)->isFree){
                header->size += nextHeader(header)->size + HEADER_SIZE;
                continue;
            }
        }
        header = nextHeader(header);
    }
    

    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    return;
}
