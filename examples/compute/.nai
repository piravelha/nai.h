
#include "../../nai.h"

int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    printf("Define a computation: ");

    String input = read_line();
    String_View computation = str_to_sv(input);

    String_View param = sv_split(&computation, "=>");

    if (!param.count) {
        log_error("Invalid computation");
        log_help("<param> => <expr>");
        return 1;
    }

    param = sv_trim(param);
    computation = sv_trim(computation);

    String out = {0};
    str_appendf(&out, "float compute(float "SV_FMT")\n", SV_ARG(param));
    str_appendf(&out, "{\n");
    str_appendf(&out, "    return "SV_FMT";\n", SV_ARG(computation));
    str_appendf(&out, "}\n");

    write_file("compute.c", str_to_sv(out));

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "main.c", "-o", "main", "-lm");

    int exit_code = cmd_run(cmd, .debug = true);

    if (exit_code == 0) {
        log_info("Compiled successfully");
    }

    return exit_code;
}


