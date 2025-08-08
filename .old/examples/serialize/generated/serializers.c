#include <stdio.h>
void _serialize_Point(Point v)
{
    printf("{");
    printf("\"x\": ");    printf("%f", v.x);
    printf(", ");
    printf("\"y\": ");    printf("%f", v.y);
    printf("}");
}

void _serialize_Person(Person v)
{
    printf("{");
    printf("\"name\": ");    printf("\"%s\"", v.name);
    printf(", ");
    printf("\"age\": ");    printf("%d", v.age);
    printf(", ");
    printf("\"point\": ");    _serialize_Point(v.point);
    printf("}");
}

