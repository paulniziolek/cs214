#include <stdio.h>
#include "nickmap.h"


int main(){
    char *s1 = "nick", *s2 = "paul", *s3 = "nick";
    map_inc(s1);
    map_set(s2,1000);
    map_inc(s3);

    printf("%d\n",map_get(s1));
    printf("%d\n",map_get(s2));
    printf("%d\n",map_get(s3));

    return 0;
}