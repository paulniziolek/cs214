#include <assert.h>
#include <string.h>
#include "mymalloc.h"

#define NUM_OBJECTS 20
#define OBJ_SIZE 100

// The main testing function to test correctness of malloc() and free(), 
// and also include testing for all expected error-producing calls of malloc() or free(). 

// EXPECTED ERRORS:
// free error: ptr not in range
//  line: 33
// malloc error: size <= 0
//  line: 77
// malloc error: size > HEAP_SIZE_IN_BYTES
//  line: 78
// malloc error: size > HEAP_SIZE_IN_BYTES
//  line: 83
// malloc error: no space found
//  line: 91
// free error: ptr already freed
//  line: 96
// free error: ptr not in range
//  line: 100
// free error: ptr not found
//  line: 103

int main() {
    // Part 0: (Error Testing, see part 3 for more)

    // Check Error: Freeing before any malloc was performed will result in an error.
    free(&main); // random address for testing purposes. 

    // Part 1: Correctness Testing with read and write.

    // We build 20 100-byte objects and write an int (representing distinct object #) to each object.
    // We then perform assertions to show correctness of each object's data.
    void *objects[NUM_OBJECTS];
    for (int i = 0; i < NUM_OBJECTS; i++) {
        objects[i] = malloc(OBJ_SIZE);
        assert(objects[i] != NULL);
        memset(objects[i], i, OBJ_SIZE);
    }

    for (int i = 0; i < NUM_OBJECTS; i++) {
        char *data = (char *)objects[i];
        for (int j = 0; j < OBJ_SIZE; j++) {
            assert(data[j] == i);
        }
        free(objects[i]);
    }

    // Part 2: Correctness Testing with coalescing.

    // We will also show coalescing by writing some objects, freeing them, then writing one 
    // bigger object and seeing that the address of our bigger object will equal our very first object (after freeing).
    // This shows that the blocks have coalesced and have properly been reassigned for the allocation of this bigger object.
    void *a, *b, *c;
    a = malloc(OBJ_SIZE);
    void *properCoalesceLocation = a;
    b = malloc(OBJ_SIZE);
    assert(a != NULL);
    assert(b != NULL);
    free(a); 
    free(b);

    // Now, bigger object c should have the same original address as a, which means b was coalesced.
    c = malloc(5 * OBJ_SIZE);
    assert(c == properCoalesceLocation);
    free(c);


    // Part 3: Error-check testing

    // Check Error: Malloc size is 0 or negative size.
    a = malloc(0);
    b = malloc(-50);
    assert(a == NULL);
    assert(b == NULL);

    // Check Error: Malloc size is greater than available heap size.
    a = malloc(2 * HEAP_SIZE);
    assert(a == NULL);

    // Check Error: Malloc memory not available in heap.
    // use up all the memory with one malloc
    a = malloc(HEAP_SIZE - HEADER_SIZE);
    assert(a != NULL);
    // any subsequent malloc will error & result in NULL ptr
    b = malloc(1);
    assert(b == NULL);

    // Check Error: Freeing same pointer twice.
    free(a);
    free(a);

    // Check Error: Specified free(ptr) is not in range of memory heap.
    int* outOfRangePtr = (int *) END;
    free(outOfRangePtr);

    // Check Error: Specified free(ptr) was not ever returned by any malloc() call, but in memory heap range
    free(heap + 57);
}
