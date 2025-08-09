#include "../../nai.h"
#include "../../nai_macro.h"


bool transform(Token_Stream *in, Token_Stream *out)
{
    if (!ts_expect_id(in, "FILE")) return false;
    if (!ts_expect_type(in, '(')) return false;

    String_View path = ts_tok(in).value;
    if (!ts_expect_type(in, TT_STRING)) return false;

    if (!ts_expect_type(in, ')')) return false;

    // trim double quotes from string literal
    path.data++;
    path.count -= 2;

    String content = {0};
    if (!read_file_sv(path, &content)) return false;

    String_View trimmed = sv_trim(str_to_sv(content));

    *out = ts_fmt("\""SV_FMT"\"", SV_ARG(trimmed));
    return true;
}

int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    Token_Stream stream = ts_replace(lex_file("main.c"), transform);
    write_file(".generated.c", stream.content);

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", ".generated.c", "-o", "main");
    cmd_run(cmd);

    return 0;
}



