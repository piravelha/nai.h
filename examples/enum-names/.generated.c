#include <stdio.h>

typedef enum {
    COLOR_RED = 69,
    COLOR_GREEN,
    COLOR_BLUE,
} Color;
const char *_names_Color(Color _) {
    switch (_) {
    case COLOR_RED:
        return "COLOR_RED";
    case COLOR_GREEN:
        return "COLOR_GREEN";
    case COLOR_BLUE:
        return "COLOR_BLUE";
    }
}
typedef enum {
    FRUIT_APPLE,
    FRUIT_BANANA,
    FRUIT_ORANGE,
} Fruit;
const char *_names_Fruit(Fruit _) {
    switch (_) {
    case FRUIT_APPLE:
        return "FRUIT_APPLE";
    case FRUIT_BANANA:
        return "FRUIT_BANANA";
    case FRUIT_ORANGE:
        return "FRUIT_ORANGE";
    }
}
void print_enums(Color color, Fruit fruit) {
    printf("color = %s, fruit = %s\n", _names_Color(color),
           _names_Fruit(fruit));
}

int main(void) {
    print_enums(COLOR_RED, FRUIT_BANANA);
    return 0;
}
