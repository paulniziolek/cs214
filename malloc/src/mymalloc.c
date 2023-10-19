#include "mymalloc.h"
#include "helper_malloc.h"
#include "helper_free.h"

double heap[HEAP_SIZE_IN_BLOCKS];

void *mymalloc_wrapper(size_t size, char *file, int line) {
    // perform error checking here

    return mymalloc(size);
}

void myfree_wrapper(void *ptr, char *file, int line) {
    // perform error checking here

    myfree(ptr);
}


/*****************************
* Debugging functions below
******************************/

// Prints current state of memory
void printMemory(){ 
    header_t *header = (header_t *) &heap[0];
    while (header != END) {
        printf("size: %d, isFree: %d, address: %p \n", ALIGN(header->size), header->isFree, header);
        header = nextHeader(header);
    }
    puts("---------------------------------------------");
}
