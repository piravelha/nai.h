#include "../../nai.h"
#include <stdio.h>

#include "compute.c"

int main(void)
{
    float value;
    
    printf("Enter a number: ");

    if (scanf("%f", &value) != 1) {
        log_error("Invalid number");
        return 1;
    }

    const char *result = sprint_number(compute(value));
    printf("Computed result is: %s\n", result);
    return 0;
}
