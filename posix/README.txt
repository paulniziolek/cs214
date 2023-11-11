Authors: Nick Belov, Paul Niziolek
netids: ndb68, pcn32

How to Build and Run:

posix can be built via running `./build.sh` which creates two executables:
`build/posix [file/directory]` will count all words within any specified files and directories. 
You should assume that if you provide the same file twice, it will double count the words. 
`build/hashmap` will perform testing on our hashmap implementation. See testing methodology for more details.

Testing methodology:

We test various aspects within the program:
Hashmap testing is done by generating all possible strings of a certain length k, and mapping a counter to each word. 
We assert that a given word has a 0 count before adding it to the hashmap. Then, we regenerate all words and assert the 
expected counts remain the same to ensure no data was lost. 
We test our posix file navigation through using an example `test/` directory in the primary working directory of the project. 
We brute force all edge cases & several subdirectories to ensure proper a output is expected. 

Hashmap Implementation:

We used a Hashmap to keep track of word counts. Abstractly, we used a hashing algorithm to partition all possible words into 
buckets, and then for each bucket we stored a linked list of key value pairs. Our map only supports string keys and long long 
values because that's all we needed.

The hashing algorithm we used can be found here:
https://cp-algorithms.com/string/string-hashing.html

Sorting:

We simply extracted all key value pairs from our map into a dynamically allocated array  and sorted them with qsort and our custom 
comparator. 

Directory Traversal:

When given a directory, the program will traverse the entirety of a directories contents and subdirectories (recursively). 

Word Parsing / File Processing:

In order to find all valid words we parse through the given files byte by byte. But to avoid making too many OS calls, we read in 
many bytes to a buffer, consider those bytes individually, then refill it once we have run out. 

To actually separate the words themselves we used a simple approach. Whenever a valid character is encountered (letters, ‘, -) we 
append it to the current working word (using our own dynamic length char array). The single exception is we will not append a - if the 
previous character was a -.

When an invalid character is encountered, or a file is ended. We consider the current word, make sure it does not end in a - (if it 
does we just remove it), and then increment its value in the map. 

Sidenotes: writeErr():

Any errors will be catched by our custom writeErr() function, which expects variadic inputs depending on a given formatted string. 
This will write() to STDERR. 
