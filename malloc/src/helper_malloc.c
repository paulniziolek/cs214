#include "helper_malloc.h"

void *mymalloc(size_t size) {
    int block_size = ALIGN(size);
    header_t *header = (header_t *) &heap[0];

    // First header not found, making new one
    if (heap[0] == 0) {
        header->size = HEAP_SIZE - HEADER_SIZE;
        header->isFree = true;
    }

    while (header != END) {
        int aligned_header_size = ALIGN(header->size);
        if (!header->isFree) {
            header = nextHeader(header);
            continue;
        }
        if (aligned_header_size >= block_size){
            // valid block found
            header_t *ptr = cut(header, size);
            return ptr + HEADER_SIZE_IN_BLOCKS;
        } 
        if (canCoalesce(header)) {
            // valid block not found, but can coalesce
            header->size = aligned_header_size + (nextHeader(header)->size) + HEADER_SIZE;
            continue;
        }
        header = nextHeader(header);
    }
    return NULL;
}

// given a block header, return the next block, or ptr to one past heap if it does not exist
header_t* nextHeader(header_t *header) {
    return (header_t *) ((char *) header + ALIGN(header->size) + HEADER_SIZE);
}

// given a header block of free memory, cut it to desired size leaving an empty suffix
// this function assumes that header->size >= size
// if thats not true *BAD* things will happen
header_t* cut(header_t *header, int size) {
    int curr_block_size = ALIGN(header->size);
    int aligned_size = ALIGN(size);

    if (curr_block_size == aligned_size){
        header->isFree = false;
        return header;
    }
    header_t *newHeader = (header_t *) ((char *) header + aligned_size + HEADER_SIZE);
    newHeader->size = curr_block_size - aligned_size - HEADER_SIZE;
    newHeader->isFree = true;

    header->size = size;
    header->isFree = false;
    return header;
}

// Determines whether the current header and the next header can coalesce.
bool canCoalesce(header_t *header) {
    return (nextHeader(header) != END && nextHeader(header)->isFree);
}
