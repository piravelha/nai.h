
#include "../../nai_macro.h"

bool csv_struct(String *s, const char *path)
{
    String csv = {0};
    TRY(read_file(path, &csv));

    String_View sv = str_to_sv(csv);

    String_View header = sv_chop(&sv, '\n');
    String_Views names = sv_split(header, ",");

    str_append(s, "struct {");

    array_foreach(String_View, name, &names) {
        str_appendf(s, "const char *%.*s;", SV(*name));
    }

    str_append(s, "}");

    return true;
}

int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    String s = {0};
    str_append(&s, "typedef ");
    if (!csv_struct(&s, "users.csv")) return 1;
    str_append(&s, " User;");

    write_file("user.h", str_to_sv(s));
    format_file("user.h");

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "main.c", "-o", "main");
    return cmd_run(cmd, .debug = true);
}


