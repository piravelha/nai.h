#include <stdio.h>
#include "gen/person.c"

int main(void)
{
    Person person = {
        .name = "Bit",
        .age = 16,
        .cool = true,
    };

    print_person(person);

    return 0;
}


