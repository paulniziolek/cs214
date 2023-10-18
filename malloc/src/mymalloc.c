#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALIGNMENT 8 // sizeof(double)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT))
#define HEADER_SIZE sizeof(header_t)/sizeof(double) + (sizeof(header_t)%sizeof(double) != 0) //size of header in blocks of 8 bytes
#define HEAP_SIZE 512
#define END &heap[HEAP_SIZE]  //one past the end of the heap

static double heap[HEAP_SIZE+1]; 

typedef struct memory_header {
    int size; 
    bool isFree;
} header_t;

//given a block header, return the next block, or ptr to one past heap if it does not exist
header_t* nextHeader(header_t *header){
    return (header_t *) &header[header->size + HEADER_SIZE];
}

//prints current state of memory, for debugging perposes
void printMemory(){ 
    header_t *header = (header_t *) &heap[0];
    while (header != END) {
        printf("size: %d, isFree: %d\n", header->size*8, header->isFree);
        header = nextHeader(header);
    }
}

//given a header block of free memory, cut it to desired size leaving an empty suffix
//this function assumes that header->size >= size
//if thats not true *BAD* things will happen
header_t* cut(header_t *header, int size){
    if(header->size == size){
        header->isFree = false;
        return header;
    }
    header_t *newHeader = (header_t *) &header[size + HEADER_SIZE];
    newHeader->size = header->size - size - HEADER_SIZE;
    newHeader->isFree = true;

    header->size = size;
    header->isFree = false;
    return header;
}

void *mymalloc(size_t size, char *file, int line) {
    //turn size into # of blocks of 8 bytes needed
    size = (size/8) + ((size%8) != 0);

    //if there is no header at the beginning of the heap, make one
    if (heap[0] == 0) {
        header_t *header = (header_t *) &heap[0];
        header->size = HEAP_SIZE - HEADER_SIZE;
        header->isFree = true;
    }

    //tricky two ptr approach
    header_t *header = (header_t *) &heap[0];
    while (header != END) {
        if (header->isFree) {
            if( header->size >= size){
                header_t *ptr = cut(header, size);
                return &ptr[HEADER_SIZE];
            } else if(nextHeader(header) != END && nextHeader(header)->isFree){
                header->size += nextHeader(header)->size + HEADER_SIZE;
                continue;
            }
        }
        header = nextHeader(header);
    }
    return NULL;
}

//currently does no error checking
//bad things can happen if given a pointer that was not returned by malloc
void myfree(void *ptr, char *file, int line) {
    header_t *header = (header_t *) (ptr);
    header[-HEADER_SIZE].isFree = true;
    return;
}
