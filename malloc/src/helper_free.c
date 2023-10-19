#include "helper_free.h"

void myfree(void *ptr) {
    header_t *header = (header_t *) (ptr) - HEADER_SIZE_IN_BLOCKS;
    int aligned_header_size = ALIGN(header->size);
    header->size = aligned_header_size;
    header->isFree = true;
    return;
}
