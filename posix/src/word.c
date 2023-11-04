#include <stdio.h>
#include <assert.h>
#include "nickmap.h"

long long global_ctr = 1;

void generate_strings(char *str, int index, int length) {
    if (index == length) {
        str[index] = '\0'; 

        assert(map_get(str) == 0);
        map_set(str, global_ctr);
        global_ctr++;
        printf("%lld\n", global_ctr);
        return;
    }

    for (char c = 'a'; c <= 'z'; c++) {
        str[index] = c;
        generate_strings(str, index + 1, length);
    }
}

int main(){
    char *s1 = "nick", *s2 = "paul", *s3 = "nick";
    char *s4 = "khhghks";
    map_inc(s1);
    map_set(s2,1000);
    map_inc(s3);
    map_set(s4, 42);
    
    printf("%d\n",map_get(s1));
    printf("%d\n",map_get(s2));
    printf("%d\n",map_get(s3));
    printf("%d\n",map_get(s4));

    int length = 5;
    char str[length + 1];  // +1 for the null terminator
    generate_strings(str, 0, length);
    printf("%s", str);

    return 0;
}