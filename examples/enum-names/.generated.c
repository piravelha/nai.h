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
        break;
    case COLOR_GREEN:
        return "COLOR_GREEN";
        break;
    case COLOR_BLUE:
        return "COLOR_BLUE";
        break;
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
        break;
    case FRUIT_BANANA:
        return "FRUIT_BANANA";
        break;
    case FRUIT_ORANGE:
        return "FRUIT_ORANGE";
        break;
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
