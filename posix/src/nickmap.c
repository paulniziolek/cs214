#include "nickmap.h"

pair_t* map[SIZE];
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
    pair_t* head = traverse_bucket(hash(str)%SIZE, str);
    return head->value;
}

void map_set(char *str, int x){
    pair_t* head = traverse_bucket(hash(str)%SIZE, str);
    head->value = x;
}

void map_inc(char *str){
    pair_t* head = traverse_bucket(hash(str)%SIZE, str);
    head->value++;
}

pair_t* traverse_bucket(ll bucket, char *str) {
    // initialize bucket
    if (map[bucket] == NULL) {
        map[bucket] = (pair_t *)malloc(sizeof(pair_t));
        map[bucket]->key = strdup(str);
        map[bucket]->value = 0;
        return map[bucket];
    }
    pair_t* head = map[bucket];
    while (strcmp(head->key, str) != 0) {
        if (head->next == NULL) {
            // add str pair to bucket
            head->next = (pair_t *)malloc(sizeof(pair_t));
            head->next->key = strdup(str);
            head->next->value = 0;
            return head->next;
        }
        head = head->next;
    }
    return head;
}
