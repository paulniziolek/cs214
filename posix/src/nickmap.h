#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1000000 //10e6
typedef long long ll;

typedef struct pair {
    char* key;
    ll value;
    struct pair* next;
} pair_t;

pair_t* traverse_bucket(ll bucket, char *str);
int map_get(char *str);
void map_set(char *str, int x);
void map_inc(char *str);
