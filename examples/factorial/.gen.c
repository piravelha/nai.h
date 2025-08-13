#include <stdio.h>

const int number = 12;

int main(void) {
    int compiletime = 479001600;
    printf("compiletime: %d\n", compiletime);

    int runtime = 1;
    {
        int _n = number;
        for (int _i = 2; _i <= _n; ++_i) {
            runtime *= _i;
        }
    }
    printf("runtime: %d\n", runtime);

    return 0;
}
