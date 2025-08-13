#include <stdio.h>

const int number = 12;

int main(void)
{
    int compiletime = factorial(12);
    printf("compiletime: %d\n", compiletime);

    int runtime = factorial(number);
    printf("runtime: %d\n", runtime);

    return 0;
}


