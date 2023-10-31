#include "nickmap.h"
#include <stdio.h>

int map[SIZE];
ll M =   29996224275833; //rly big prime

ll hash(char *str){
    ll hash = 0, p = 69420, mult = p;
    for(int i = 0; str[i] != '\0'; i++){
        hash += (str[i] * mult) % M, hash %= M;
        mult *= p, mult %= M;
    }
    return hash;
}

int map_get(char *str){
    return map[hash(str)%SIZE];
}

void map_set(char *str, int x){
    map[hash(str)%SIZE] = x;
}

void map_inc(char *str){
    map[hash(str)%SIZE]++;
}