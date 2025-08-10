#include <stdio.h>

typedef enum { COLOR_RED , COLOR_GREEN , COLOR_BLUE , } Color ; const char * _names_Color [ ] = { [ COLOR_RED ] = "COLOR_RED" , [ COLOR_GREEN ] = "COLOR_GREEN" , [ COLOR_BLUE ] = "COLOR_BLUE" , } ; int main(void)
{
    Color color = COLOR_BLUE;
    printf("%s\n", _names_Color [ color ] );
}


