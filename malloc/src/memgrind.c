#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "mymalloc.h"

/**
4. Two more stress tests of your design. Document these in your README.
Your program should run each task 50 times, recording the amount of time needed for each task,
and then reporting the average time needed for each task. 

You may use gettimeofday() or similar functions to obtain timing information.
**/

// func prototypes
void testNick();
void test1();
void test2();
void test3();
void test4();
void test5();
bool isMemCleared();

int main(void) {
    /*
    printf("Test 1 results: ");
    test1();
    printf("Test 2 results: ");
    test2();
    printf("Test 3 results: ");
    test3();
    */
    testNick();

    return 0;
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
    for(int i = 0; i < 120; i++) {
        char *ptr = malloc(1);
        free(ptr);
    }
    printf("MemClear?: %d\n", isMemCleared());
}

// Use malloc() to get 120 1-byte objects, storing the pointers in an array, then use free() to
// deallocate the chunks.
void test2() {
    char *ptrArray[120];
    for(int i = 0; i < 120; i++) {
        ptrArray[i] = malloc(1);
    }
    for(int i = 0; i < 120; i++) {
        free(ptrArray[i]);
    }
    printf("MemClear?: %d\n", isMemCleared());
}

// Create an array of 120 pointers. Repeatedly make a random choice between allocating a 1-byte
// object and adding the pointer to the array and deallocating a previously allocated object (if
// any), until you have allocated 120 times. Deallocate any remaining objects.
void test3() {
    char *ptrArray[120];
    int allocated[120] = {0};
    int loc = 0;
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
    printf("MemClear?: %d\n", isMemCleared());
}

void test4() {
   
}

void test5() {

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