1. descriptions of our custom stress tests
2. describe error reporting and design in read me (also we could change the format of error msgs)
3.  README file containing the name and netID of both partners, 
    your test plan, descriptions of your test programs 
    (including any arguments they may take), and any 
    design notes you think are worth pointing out. This should be a plain text file.


Nick Belov, Paul Niziolek
ndb68, pcn32

1. General Design

As required the actualy memory is stored in an array of doubles length 512. All
auxilary information used for managing the memory is stored in structs called header_t. 

The header_t struct (defined in src/mymalloc.h) contains the following information:
1. unsigned integer size of the block
2. boolean is_free
its size as tested on our machines is 8 bytes.

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

    1. If the user tries to malloc 0 bytes. We thought 99% of time this would be a mistake.

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


