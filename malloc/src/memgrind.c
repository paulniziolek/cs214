#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include "mymalloc.h"

/**
4. Two more stress tests of your design. Document these in your README.
Your program should run each task 50 times, recording the amount of time needed for each task,
and then reporting the average time needed for each task. 

You may use gettimeofday() or similar functions to obtain timing information.
**/

// func prototypes
void testNick();
void runTests();
void test1();
void test2();
void test3();
void test4();
void test5();
bool isMemCleared();

const int T = 50;

int main(void) {
    runTests();

    return 0;
}

void runTests() {
    test1();
    test2();
    test3();
    test4();
    test5();
}

// just me (nick) testing on my machine
void testNick(){ 
    void *a,*b,*c,*d,*e,*f;
    a = malloc(10);
    printMemory();
    b = malloc(20);
    printMemory();
    free(b);
    printMemory();
    c = malloc(30);
    printMemory();
    free(a),free(c);
    printMemory();
    d = malloc(100);
    printMemory();
    e = malloc(300);
    f = malloc(3600);
    free(e);
    printMemory();
    e = malloc(500);
    a = malloc(50000);
    printMemory();

}

// malloc() and immediately free() a 1-byte object, 120 times.
void test1() {
    struct timeval start, end;
    double totalTime = 0;

    for (int i = 0; i < T; i++) {
        gettimeofday(&start, NULL);
        for(int i = 0; i < 120; i++) {
            char *ptr = malloc(1);
            free(ptr);
        }
        gettimeofday(&end, NULL);
        totalTime += (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    }

    printf("Test 1 results: \n");
    printf("MemClear: %d, Avg Time: %f microseconds\n", isMemCleared(), totalTime/T);
    puts("--------------------------");
    initializeMemory();
}

// Use malloc() to get 120 1-byte objects, storing the pointers in an array, then use free() to
// deallocate the chunks.
void test2() {
    struct timeval start, end;
    double totalTime = 0;
    char *ptrArray[120];
    for (int i = 0; i < T; i++) {
        gettimeofday(&start, NULL);
        for(int i = 0; i < 120; i++) {
            ptrArray[i] = malloc(1);
        }
        for(int i = 0; i < 120; i++) {
            free(ptrArray[i]);
        }
        gettimeofday(&end, NULL);
        totalTime += (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    }
    printf("Test 2 results: \n");
    printf("MemClear: %d, Avg Time: %f microseconds\n", isMemCleared(), totalTime/T);
    puts("--------------------------");
    initializeMemory();
}

// Create an array of 120 pointers. Repeatedly make a random choice between allocating a 1-byte
// object and adding the pointer to the array and deallocating a previously allocated object (if
// any), until you have allocated 120 times. Deallocate any remaining objects.
void test3() {
    struct timeval start, end;
    double totalTime = 0;
    char *ptrArray[120];
    int allocated[120] = {0};
    int loc = 0;
    for (int i = 0; i < T; i++) {
        gettimeofday(&start, NULL);
        for(int i = 0; i < 120; i++) {
            if(loc == 0 || (rand() % 2 == 0 && loc < 120)) {
                //printf("alloc loc=%d\n", loc);
                ptrArray[loc] = malloc(1);
                allocated[loc] = 1;
                loc++;
            } else {
                loc--;
                //printf("free loc=%d\n", loc);
                free(ptrArray[loc]);
                allocated[loc] = 0;
            }
        }
        //printf("Process is done.\n");
        for(int i = 0; i < 120; i++) {
            if(allocated[i] == 1) {
                free(ptrArray[i]);
            }
        }
        gettimeofday(&end, NULL);
        totalTime += (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    }
    printf("Test 3 results: \n");
    printf("MemClear: %d, Avg Time: %f microseconds\n", isMemCleared(), totalTime/T);
    puts("--------------------------");
    initializeMemory();
}

// We will allocate blocks of random sizes, between 1 and 64 bytes, 50 times. Then we will randomly free some of these blocks. 
// Then attempt to allocate a large block of memory of 500 bytes (this might not fit). Then we clear all memory.
void test4() {
    struct timeval start, end;
    double totalTime = 0;
    const int ARRAY_SIZE = 50;
    const int MAX_MALLOC = 64;
    char *ptrArray[50];
    for (int i = 0; i < T; i++) {
        gettimeofday(&start, NULL);
        for (int j = 0; j < ARRAY_SIZE; j++) {
            ptrArray[j] = malloc((rand() % MAX_MALLOC) + 1);
        }
        for (int j = 0; j < ARRAY_SIZE / 2; j++) {
            int index = rand() % ARRAY_SIZE;
            if (ptrArray[index] != NULL) {
                free(ptrArray[index]);
                ptrArray[index] = NULL;
            }
        }
        char *largePtr = malloc(500);
        if (largePtr) {
            free(largePtr);
        }
        for (int j = 0; j < ARRAY_SIZE; j++) {
            if (ptrArray[j]) {
                free(ptrArray[j]);
            }
        }
        gettimeofday(&end, NULL);
        totalTime += (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    }
    printf("Test 4 results: \n");
    printf("MemClear: %d, Avg Time: %f microseconds\n", isMemCleared(), totalTime/T);
    puts("--------------------------");
    initializeMemory();
}

// We will allocate 1-byte chunks until memory is filled (when malloc() returns NULL), 
// then free() each chunk in reverse order, then malloc() for the majority of the memory, forcing coalescing of chunks.
void test5() {
    struct timeval start, end;
    double totalTime = 0;
    const int MAX_MALLOC = HEAP_SIZE - HEADER_SIZE;
    char *ptrArray[HEAP_SIZE / HEADER_SIZE];
    for (int i = 0; i < T; i++) {
        gettimeofday(&start, NULL);
        for (int i = 0; i < HEAP_SIZE / (HEADER_SIZE + ALIGNMENT); i++) {
            ptrArray[i] = malloc(1);
        }
        for (int i = HEAP_SIZE / (HEADER_SIZE + ALIGNMENT) - 1; i >= 0; i--) {
            free(ptrArray[i]);
        }
        char *big = malloc(MAX_MALLOC);
        if (big != NULL) {
            free(big);
        }
        gettimeofday(&end, NULL);
        totalTime += (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec);
    }
    printf("Test 5 results: \n");
    printf("MemClear: %d, Avg Time: %f microseconds\n", isMemCleared(), totalTime/T);
    puts("--------------------------");
    initializeMemory();
}

bool isMemCleared() {
    header_t *header = (header_t *) &heap[0];;
    while (header != END) {
        if (!header->isFree) {
            return false;
        }
        header = nextHeader(header);
    }
    return true;
}
