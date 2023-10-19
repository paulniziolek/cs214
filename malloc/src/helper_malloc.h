#ifndef HELPER_MALLOC_H
#define HELPER_MALLOC_H

#include "mymalloc.h"

// User-facing functions
void *mymalloc(size_t size);

// Internal functions
header_t* nextHeader(header_t *header);
header_t* cut(header_t *header, int size);
bool canCoalesce(header_t *header);

#endif
