#include <stdlib.h>
#include "mymalloc.h"
#include "mymalloc.c" //wont compile without this
/**

1. malloc() and immediately free() a 1-byte object, 120 times.

2. Use malloc() to get 120 1-byte objects, storing the pointers in an array, then use free() to
deallocate the chunks.

3. Create an array of 120 pointers. Repeatedly make a random choice between allocating a 1-byte
object and adding the pointer to the array and deallocating a previously allocated object (if
any), until you have allocated 120 times. Deallocate any remaining objects.

4. Two more stress tests of your design. Document these in your README.
Your program should run each task 50 times, recording the amount of time needed for each task,
and then reporting the average time needed for each task. 

You may use gettimeofday() or similar functions to obtain timing information.

**/

int main(void) {
    malloc(5);
    
    return 0;
}
