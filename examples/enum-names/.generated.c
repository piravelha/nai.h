#include <stdio.h>

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
} Color;
const char *_names_Color[] = {
    [COLOR_RED] = "COLOR_RED",
    [COLOR_GREEN] = "COLOR_GREEN",
    [COLOR_BLUE] = "COLOR_BLUE",
};
typedef enum {
    FRUIT_APPLE,
    FRUIT_BANANA,
    FRUIT_ORANGE,
} Fruit;
const char *_names_Fruit[] = {
    [FRUIT_APPLE] = "FRUIT_APPLE",
    [FRUIT_BANANA] = "FRUIT_BANANA",
    [FRUIT_ORANGE] = "FRUIT_ORANGE",
};
void print_enums(Color color, Fruit fruit) {
    printf("color = %s, fruit = %s\n", _names_Color[color],
           _names_Fruit[fruit]);
}

int main(void) {
    print_enums(COLOR_BLUE, FRUIT_BANANA);
    return 0;
}
