#include <stdio.h>
#include "tag.c"

void foo(void) TAG("foo is tagged!")
{
    return;
}

int main(void)
{
    printf("%s\n", TAG_OF(foo));
    return 0;
}


