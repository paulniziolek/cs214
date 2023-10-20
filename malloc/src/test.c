#include <assert.h>
#include <string.h>
#include "mymalloc.h"

// The main testing function to test correctness of malloc() and free(), 
// and also include testing for all expected error-producing calls of malloc(). 
#define NUM_OBJECTS 5
#define OBJ_SIZE 100

int main() {
    // Part 1: Correctness Testing
    printf("bruh");
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
    printf("works");

    


}