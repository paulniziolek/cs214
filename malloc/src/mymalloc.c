#include "mymalloc.h"

double heap[HEAP_SIZE_IN_BLOCKS];

void *mymalloc_wrapper(size_t size, char *file, int line) {
    // Error: Malloc size less than or equal to 0. Result in stderr and return NULL
    if(size <= 0){
        fprintf(stderr,"malloc error: size <= 0\n file: %s\n line: %d\n",file,line);
        return NULL;
    }
    // Error: Malloc size greater than specified HEAP_SIZE - HEADER_SIZE. Result in stderr and return NULL
    if(size > HEAP_SIZE - HEADER_SIZE){
        fprintf(stderr,"malloc error: size > HEAP_SIZE_IN_BYTES\n file: %s\n line: %d\n",file,line);
        return NULL;
    }
    void *ptr = mymalloc(size);
    // Error: Malloc not properly allocated. Result in stderr and return NULL
    if(ptr == NULL){
        fprintf(stderr,"malloc error: no space found\n file: %s\n line: %d\n",file,line);
        return NULL;
    }
    return ptr;
}

void myfree_wrapper(void *ptr, char *file, int line) {
    // Error: Pointer specified is not in heap range (external pointer). Result in stderr
    if((char*)ptr < (char*)&heap[0] || (char*)ptr >= (char*)END){
        fprintf(stderr,"free error: ptr not in range\n file: %s\n line: %d\n",file,line);
        return;        
    }
    // Error: Free used before any malloc & heap initialization. Result in stderr
    if(heap[0]==0){
        fprintf(stderr,"free error: heap not initialized yet\n file: %s\n line: %d\n",file,line);
        return;        
    }

    // loop over current blocks
    header_t *header = (header_t*)&heap[0];
    header_t *need = (header_t*)ptr - HEADER_SIZE_IN_BLOCKS;
    while(header != END){
        if(header==need){
            // Error: Attempting to free block that has already been freed. Result in stderr
            if(header->isFree){
                fprintf(stderr,"free error: ptr already freed\n file: %s\n line: %d\n",file,line);
                return;
            }
            // block is valid, go ahead and free it
            myfree(ptr);
            return;
        }
        header=nextHeader(header);
    }

    // Error: Specified pointer not found in available heap memory range. Either pointer was very old
    // and memory structure has changed, or it was never returned by any malloc. Result in stderr
    fprintf(stderr,"free error: ptr not found\n file: %s\n line: %d\n",file,line);
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
