#include "serialize.c"


SERIALIZE typedef struct {
    float x;
    float y;
} Point;


SERIALIZE typedef struct {
    const char *name; 
    int age;
    int income; ATTRIBUTE(hidden)
    Point point;
} Person;


#include "generated/serializers.c"

int main(void)
{
    Person person = {
        .name = "Bit",
        .age = 16,
        .income = 1200,
        .point = {
            .x = 34,
            .y = 35,
        },
    };

    serialize(Person, person);

    return 0;
}



