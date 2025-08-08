#include <stdbool.h>

typedef struct {
    const char *name;
    int age;
} Person;

define print(type) _print_##type

void _print_Person(Person p)
{
    printf("(Person) {\n");
    printf("    .name = \"%s\",\n", p.name);
    printf("    .age = %d,\n", p.age);
    printf("}\n");
}


