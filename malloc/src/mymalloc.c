#include "mymalloc.h"

double heap[HEAP_SIZE_IN_BLOCKS];

void *mymalloc_wrapper(size_t size, char *file, int line) {
    //checking is size makes sense
    if(size <= 0){
        fprintf(stderr,"malloc error: size <= 0\n file: %s\n line: %d\n",file,line);
        return NULL;
    }
    if(size > HEAP_SIZE){
        fprintf(stderr,"malloc error: size > HEAP_SIZE_IN_BYTES\n file: %s\n line: %d\n",file,line);
        return NULL;
    }
    void *ptr = mymalloc(size);
    if(ptr == NULL){
        fprintf(stderr,"malloc error: no space found\n file: %s\n line: %d\n",file,line);
        return NULL;
    }
    return ptr;
}

void myfree_wrapper(void *ptr, char *file, int line) {
    //check if ptr is in range
    if((char*)ptr < (char*)&heap[0] || (char*)ptr >= (char*)END){
        fprintf(stderr,"free error: ptr not in range\n file: %s\n line: %d\n",file,line);
        return;        
    }
    //heap not initialized edge case
    if(heap[0]==0){
        fprintf(stderr,"free error: heap not initialized yet\n file: %s\n line: %d\n",file,line);
        return;        
    }

    //loop over current blocks
    header_t *header = (header_t*)&heap[0];
    header_t *need = (header_t*)ptr - HEADER_SIZE_IN_BLOCKS;
    while(header != END){
        if(header==need){
            //already freed this block
            if(header->isFree){
                fprintf(stderr,"free error: ptr already freed\n file: %s\n line: %d\n",file,line);
                return;
            }
            //block is valid, go ahead and free it
            myfree(ptr);
            return;
        }
        header=nextHeader(header);
    }

    //ptr not found
    //either it was very old and block structure had changed
    //or it was never returned by malloc in the first place
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
