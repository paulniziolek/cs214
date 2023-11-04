#include "hashmap.h"

typedef long long ll;

typedef struct pair {
    char* key;
    ll value;
    struct pair* next;
} pair_t;

struct HashMap {
    pair_t* data[SIZE];
};

static ll hash(const char *key);
static pair_t* traverse_bucket(HashMap *map, ll bucket, const char *str);

ll M = 29996224275833; //rly big prime

HashMap* initMap() {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map==NULL) {
        // TODO: panic
        return map;
    }
    for (int i = 0; i < SIZE; i++) {
        map->data[i] = NULL;
    }
    return map;
}

int mapGet(HashMap *map, const char *str){
    pair_t* head = traverse_bucket(map, hash(str)%SIZE, str);
    return head->value;
}

void mapSet(HashMap *map, const char *str, int x){
    pair_t* head = traverse_bucket(map, hash(str)%SIZE, str);
    head->value = x;
}

void mapInc(HashMap *map, const char *str){
    pair_t* head = traverse_bucket(map, hash(str)%SIZE, str);
    head->value++;
}

void mapDestroy(HashMap *map) {
    for (int i=0; i<SIZE; i++) {
        pair_t* head = map->data[i];
        while (head != NULL) {
            pair_t* curr = head;
            head = head->next;
            free(curr->key);
            free(curr);
        }
    }
    free(map);
    map = NULL;
}

ll hash(const char *key){
    ll hash = 0, p = 69420, mult = p;
    for(int i = 0; key[i] != '\0'; i++){
        hash += (key[i] * mult) % M, hash %= M;
        mult *= p, mult %= M;
    }
    return hash;
}

pair_t* traverse_bucket(HashMap *map, ll bucket, const char *str) {
    // initialize bucket
    if (map->data[bucket] == NULL) {
        map->data[bucket] = (pair_t *)malloc(sizeof(pair_t));
        map->data[bucket]->key = strdup(str);
        map->data[bucket]->value = 0;
        return map->data[bucket];
    }
    pair_t* head = map->data[bucket];
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
