---

# Nai

Nai is a build system implemented entirely in C.  
It allows projects to define their own build logic and source transformations directly in C code.  

---

## Features

- Implemented entirely in C99 (no dependencies apart from libc)
- Build logic expressed as normal imperative C code.
- Self-rebuilding executables.
- Supports custom preprocessing and source transformations (see nai_macro.h).
- Portable across any environment with a C99 compiler.

---


## Example (from examples/simple/nai.c)

```c

#include "nai.h"

int main(int argc, char **argv)
{
    char *mode;
    Cmd cmd = {0};
    int exit_code;

    REBUILD(argc, argv);

    arg_shift(&argc, &argv);

    if (!argc) {
        log_error("Expected compilation mode");
        log_help("(debug | release)");
        return 1;
    }

    *mode = arg_shift(&argc, &argv);
    cmd_append(&cmd, "cc", "main.c", "-o", "main");

    if (!strcmp(mode, "debug")) {
        cmd_append(&cmd, "-O0", "-ggdb");
    } else if (!strcmp(mode, "release")) {
        cmd_append(&cmd, "-O2");
    } else {
        log_error("Invalid compilation mode: %s", mode);
        return 1;
    }

    exit_code = cmd_run(cmd, .debug = true);

    if (exit_code == 0) {
        log_info("Compiled successfully");
    } else {
        log_info("Failed compiling");
    }

    return exit_code;
}



```


---

Usage

1. Compile nai.c once manually:

```sh
cc nai.c -o nai
```


2. Run nai to process and build the project:

```sh
./nai
```




---

License

MIT License.

---

