#include "../../nai.h"

//

int factorial(int n)
{
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}


int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    arg_shift(&argc, &argv);

    if (!argc) {
        printf("Expected factorial input\n");
        return 1;
    }

    char *input = arg_shift(&argc, &argv);
    int value = atoi(input);

    String_Builder sb = {0};
    sb_printf(&sb, "#define VALUE %d\n", value);
    sb_printf(&sb, "#define RESULT %d\n", factorial(value));
    write_file("gen/result.c", sb);

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "main.c");
    cmd_append(&cmd, "-o", "build/main");

    cmd_run(cmd);

    return 0;
}


