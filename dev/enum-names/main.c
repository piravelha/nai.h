#include <stdio.h>

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
} Color;

int main(void)
{
    Color color = COLOR_BLUE;
    printf("%s\n", [Color: color]);
}


