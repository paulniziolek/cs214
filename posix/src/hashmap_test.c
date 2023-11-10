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

int main(){
    map = initMap();
    char *s1 = "nick", *s2 = "paul", *s3 = "nick";
    char *s4 = "khhghks";
    mapInc(map, s1);
    mapSet(map, s2,1000);
    mapInc(map, s3);
    mapSet(map, s4, 42);
    
    printf("%d\n",mapGet(map, s1));
    printf("%d\n",mapGet(map, s2));
    printf("%d\n",mapGet(map, s3));
    printf("%d\n",mapGet(map, s4));

    int length = 5;
    char str[length + 1];  // +1 for the null terminator
    generate_strings(str, 0, length);
    printf("%s\n", str);
    printf("%lld\n", global_ctr);

    mapDestroy(map);
    return 0;
}