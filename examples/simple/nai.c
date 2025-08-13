
#include "../../nai.h"

int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    arg_shift(&argc, &argv);

    if (!argc) {
        log_error("Expected compilation mode");
        log_help("(debug | release)");
        return 1;
    }

    char *mode = arg_shift(&argc, &argv);

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "main.c", "-o", "main");

    if (!strcmp(mode, "debug")) {
        cmd_append(&cmd, "-O0", "-ggdb");
    } else if (!strcmp(mode, "release")) {
        cmd_append(&cmd, "-O2");
    } else {
        log_error("Invalid compilation mode: %s", mode);
        log_help("(debug | release)");
        return 1;
    }

    int exit_code = cmd_run(cmd, .debug = true);

    if (exit_code == 0) {
        log_info("Compiled successfully");
    } else {
        log_info("Failed compiling");
    }

    return exit_code;
}


