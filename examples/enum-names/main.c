#include <stdio.h>

typedef enum {
    COLOR_RED = 69,
    COLOR_GREEN,
    COLOR_BLUE,
} Color;

typedef enum {
    FRUIT_APPLE,
    FRUIT_BANANA,
    FRUIT_ORANGE,
} Fruit;

void print_enums(Color color, Fruit fruit)
{
    printf("color = %s, fruit = %s\n",
           nameof(Color, color),
           nameof(Fruit, fruit));
}

int main(void)
{
    print_enums(COLOR_RED, FRUIT_BANANA);
    return 0;
}


