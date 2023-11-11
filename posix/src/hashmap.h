#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Implement hashmap resizing to automatically scale with the number of inputs.
#define MAP_SIZE 1000000 //1e6

// Implements a simple hashmap, with the operations specified below. 
// Note that for any get, set, and increment call the hashmap will automatically
// initialize a zero-valued pair first if none exist. 

typedef struct pair {
    char* key;
    long long value;
    struct pair* next;
} pair_t;

typedef struct key_value{
    char* key;
    long long value;
} key_value;

typedef struct HashMap {
    pair_t* data[MAP_SIZE];
    long long size;
} HashMap;


// Initializes a hashmap & returns its address in memory
// It is the caller's responsibility to free the hashmap with mapDestroy().
HashMap* initMap();

// Attempts to get the value for the specified str. 
int mapGet(HashMap *map, const char *str);

//returns an array of key_value pairs
//caller of this function must remember to FREE the returned array
key_value* mapKeyValuePairs(HashMap *map);

// Attempts to set the value for a specified str.
void mapSet(HashMap *map, const char *str, int x);

// Attempts to increment the value of the specified str by 1.
void mapInc(HashMap *map, const char *str);

// Will free the hashmap from memory.
void mapDestroy(HashMap *map);

#endif
