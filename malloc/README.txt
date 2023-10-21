Authors: Nick Belov, Paul Niziolek
netids: ndb68, pcn32

https://github.com/paulniziolek/cs214

1. General Design

As required the actualy memory is stored in an array of doubles length 512. All
auxilary information used for managing the memory is stored in structs called header_t. 

The header_t struct (defined in src/mymalloc.h) contains the following information:
1. integer user allocated size of the block
2. boolean is_free
its size as tested on our machines is 8 bytes.

Note that we store the user allocated size of the block in the header instead of the
alligned block size. This was done for future portability, in case we ever wanted to 
return the size of the block to the user. When we need the alligned block size we just 
do a simple calculation.

Headers are stored in memory directly before the block they are managing. Since after
each block there is either a header or the end of the array, we can use the size of the
current header to find the next header. 

We have a function that does this called nextHeader defined in src/helper_malloc.c. If 
nextHeader is called on the last header in the array, it returns END. END is a header_t
pointer one past the end of the memory array, #defined in src/mymalloc.h.

With nextHeader and END it is easy to iterate through all the headers memory linked
list style. This is our main method of traversing the headers seen in many places in the
code.

Note: both mymalloc and myfree are always called by our wrapper functions which do all
error checking which will be discussed later.

a. void* mymalloc(size_t size)  //size_t is an unsigned integer type
    - mymalloc first checks if heap[0] == 0, which would mean that memory has not
    been initialized yet. In this case a header is created at heap[0] with maximum size.

    -mymalloc then rounds up the size up to fit the alignment requirments.

    - Then mymalloc iterates through the headers in the method mentioned before, coalescing
    adjacent free blocks as it goes. If a large enough block is found/created via coalescing,
    the block is cut to the correct size (see cut() in src/helper_malloc.c) and a ptr to the 
    payload is returned.

    -If no large enough block is found mymalloc returns NULL

b. void myfree(void *ptr)
    -since error checking is already done by our wrapper function, myfree simply goes
    back by one alignment to find the header of the block it is freeing. It then sets
    the header's is_free to true.

c. Notes on our modularity
    -in mymalloc.h we used #define to define a large number of constants in a way that
    yields itself to future modification. For example, if we wanted to change the size
    of the memory array, we could simply change the value of HEAP_SIZE and all the code
    would still work.
    
Stress Tests:

After running ./build.sh, you can run the stress test by running ./build/main 
We have 5 stress tests, the 3 required and 2 of our own. All tests are run 50 times
and then report the average time in microseconds.

-test1 (given): 
    Simply malloc and free a 1 byte object 120 times.

-test2 (given): 
    Malloc 120 1byte objects, store them in an array, then free them.

-test3 (given): 
    Create an array of 120 pointers. Repeatedly make a random choice between allocating 
    a 1-byte object and adding the pointer to the array and deallocating a previously 
    allocated object (if any), until you have allocated 120 times. Deallocate any 
    remaining objects.

test4 (ours):
    We allocate blocks of random sizes, between 1 and 64 bytes, 50 times. Then we
    randomly free some of these blocks. Then attempt to allocate a large block of memory 
    of 500 bytes (this might not fit). Then we clear all memory.

notes on test4:
    We were interested in seeing how fragmentation and coalescing would affect our 
    perfomance. This test actually was abnormally fast. We credit this to the fact that
    both malloc and the error checking on free are O(blocks) so if we make large blocks
    the program actually speeds up!

test5 (ours):
    We will allocate 1-byte chunks until memory is filled (when malloc() returns NULL), then 
    free() each chunk in reverse order, then malloc() for the majority of the memory, forcing 
    coalescing of chunks.

notes on test5:
    Here we decided to give the program the worst possible case that might actually come 
    up in a real program. Maximizing blocks is important since everything takes O(blocks).
    It's also we free the blocks in reverse order since the error checking on free 
    traverses all the way up to the block being freed. As predicted this test was an order
    of magnitude slower than the others.

Error Reporting:

Any errors found are printed to stderr. We decided NOT to terminate the program
on error to let the user decide out to handle them.

In src/mymalloc.h we have:
#define malloc(s) mymalloc_wrapper(s, __FILE__, __LINE__)
#define free(p) myfree_wrapper(p, __FILE__, __LINE__)
These macros are used to call our wrapper functions which do all error checking. Both 
of these wrapper functions are defined in src/mymalloc.c.

-mymalloc_wrapper
In any error case it prints an error message with line number, file, and description 
of error to stderr then returns NULL.

mymalloc_wrapper reports 3 different types of errors:

    1. If the user tries to malloc <= 0 bytes. We thought 99% of time this would be a mistake.

    2. If the user tries to malloc more than the maximum size of a block. The raw malloc 
    function would just return null in this case, but we thought it would be helpful to
    report this error to the user so they don't try useless things.

    3. If malloc does not find a free block of the correct size. 
        
-myfree_wrapper
In any error case it prints an error message with line number, file, and description.

my_freewrapper reports 4 types of errors.

    1. The given ptr did not come from our malloc. This is checked just by making sure the
    ptr is within the bounds of the memory array.

    2. If the user calls free before the memory array has been initialized. We had to include
    this check because the following error check involves iterating over the headers in the 
    memory array. 

    3. If the user tries to free a ptr that is not the start of a block. We check this by 
    iterating through the headers to see if we find a header consistent with the given 
    pointer.

    4. If the user tries to free a block that has already been freed. Since we already 
    find the block in the previous error check, here we just check if the block is free.

Notes on error checking for free:
We made the decision to iterate over all headers in our error checking to make sure given
pointer is leginimate. This way even if the user puts a fake header in a payload and tries 
to spoof a memory block we will still catch the error. We did consider other methods like
giving each header a UUID or blocking off some section of the memory to maintain a 
contigious list of pointers to leginimate headers, but our method was easiest to impliment.

The perfomance hit in perfomance is not such a big deal since the memory array is only len 
512.


Correctness Testing:
After running ./build.sh, you can run the stress test by running ./build/test 
Our correctness test will test various errors that our code produces, as well as 
correctness of the allocation of various valued objects.

A quick walkthrough of how we correctness test:
- Parts 0 & 3 will test each and every single error, and is expected to return the following errors (listed below).
- Part 1 will perform a correctness test on the read and write using our malloc. We read and write 20 100-byte object, 
each having values stored in the object's memory, valued 1-20 respectively. We then verify that each object has the expected
memory by calling assertions over each object, asserting the memory of object i equals to int i. In the end, we free all the objects.
- Part 2 will perform a coalesce correctness test, by first calling malloc() on 2 100-byte objects, then storing the memory address 
of the first object. If we free both of these objects, and malloc() a larger 500-byte object, we should guarantee a coalesce on the original
2 (now free) objects and compare and assert the memory addresses of the very first object and the very last (largest) object. 
This verifies our coalesce strategy

From parts 0 and 3, we intentionally will reach error states from our use of malloc() or free(). We document each of the expected errors in test.c and below:
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

These are the expected errors the program should run into, as described by the error conditions of malloc() and free() earlier in this README file. 

