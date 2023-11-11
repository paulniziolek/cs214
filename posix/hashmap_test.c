#include <stdio.h>
#include <assert.h>
#include "hashmap.h"

long long global_ctr = 1;
HashMap* map;

void generate_strings(char *str, int index, int length) {
    if (index == length) {
        str[index] = '\0'; 

        assert(mapGet(map, str) == 0);
        mapSet(map, str, global_ctr);
        global_ctr++;
        return;
    }

    for (char c = 'a'; c <= 'z'; c++) {
        str[index] = c;
        generate_strings(str, index + 1, length);
    }
}

void generate_strings_for_assertion(char *str, int index, int length) {
    if (index == length) {
        str[index] = '\0'; 

        assert(mapGet(map, str) == global_ctr);
        global_ctr++;
        return;
    }

    for (char c = 'a'; c <= 'z'; c++) {
        str[index] = c;
        generate_strings_for_assertion(str, index + 1, length);
    }
}

int main(){
    map = initMap();

    int length = 4;
    char str[length + 1];  // +1 for the null terminator
    generate_strings(str, 0, length);
    printf("%s\n", str);
    printf("%lld\n", global_ctr);

    global_ctr = 1;
    generate_strings_for_assertion(str, 0, length);
    printf("%lld\n", global_ctr);

    mapDestroy(map);
    return 0;
}