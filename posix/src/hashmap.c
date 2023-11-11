#include "hashmap.h"
typedef long long ll;

static ll hash(const char *key);
static pair_t* traverse_bucket(HashMap *map, ll bucket, const char *str);

ll M = 29996224275833; //rly big prime

HashMap* initMap() {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map==NULL) {
        // TODO: panic
        return map;
    }
    map->size = 0;
    for (int i = 0; i < MAP_SIZE; i++) {
        map->data[i] = NULL;
    }
    return map;
}

int mapGet(HashMap *map, const char *str){
    pair_t* head = traverse_bucket(map, hash(str)%MAP_SIZE, str);
    return head->value;
}

void mapSet(HashMap *map, const char *str, int x){
    pair_t* head = traverse_bucket(map, hash(str)%MAP_SIZE, str);
    head->value = x;
}

void mapInc(HashMap *map, const char *str){
    pair_t* head = traverse_bucket(map, hash(str)%MAP_SIZE, str);
    head->value++;
}

void mapDestroy(HashMap *map) {
    for (int i=0; i<MAP_SIZE; i++) {
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
        map->size++;
        map->data[bucket] = (pair_t *)malloc(sizeof(pair_t));
        map->data[bucket]->key = strdup(str);
        map->data[bucket]->value = 0;
        map->data[bucket]->next = NULL;
        return map->data[bucket];
    }
    pair_t* head = map->data[bucket];
    while (strcmp(head->key, str) != 0) {
        if (head->next == NULL) {
            // add str pair to bucket
            map->size++;
            head->next = (pair_t *)malloc(sizeof(pair_t));
            head->next->key = strdup(str);
            head->next->value = 0;
            head->next->next = NULL;
            return head->next;
        }
        head = head->next;
    }
    return head;
}

key_value *mapKeyValuePairs(HashMap *map){
    key_value *pairs = (key_value *)malloc(map->size * sizeof(key_value));
    ll index = 0;
    for (int i=0; i<MAP_SIZE; i++) {
        pair_t* head = map->data[i];
        while (head != NULL) {
            pairs[index].key = head->key;
            pairs[index].value = head->value;
            index++;
            head = head->next;
        }
    }
    return pairs;
}
