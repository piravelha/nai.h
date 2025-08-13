#include "../../nai_macro.h"


bool transform(Token_Stream *in, Token_Stream *out)
{
    String_View name = ts_value(*in);
    TRY(ts_expect_type(in, TT_ID));
    TRY(ts_expect_type(in, '='));

    TRY(ts_expect_value(in, "factorial"));

    Token_Stream args = {0};
    TRY(ts_balanced(in, &args, "(", ")"));
    TRY(ts_expect_type(in, ';'));

    String_View value = ts_value(args);
    if (ts_expect_type(&args, TT_INTEGER)) {
        TRY(ts_expect_eof(&args));

        int n = sv_parse_int(value);
        int x = 1;
        for (int i = 2; i <= n; ++i) {
            x *= i;
        }

        *out = ts_fmt("%.*s = %d;", SV(name), x);
        return true;
    }
    
    String code = {0};
    str_appendf(&code, "%.*s = 1;", SV(name));
    str_append(&code, "{");
    str_appendf(&code, "int _n = %.*s;", SV(ts_render(args)));
    str_append(&code, "for (int _i = 2; _i <= _n; ++_i) {");
    str_appendf(&code, "%.*s *= _i;", SV(name));
    str_append(&code, "}");
    str_append(&code, "}");

    *out = ts_fmt("%.*s", SV(code));
    return true;
}


int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    Token_Stream stream = lex_file("main.c");
    stream = ts_replace(stream, transform);
    write_file(".gen.c", stream.content);
    format_file(".gen.c");

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", ".gen.c", "-o", "main");
    return cmd_run(cmd, .debug = true);
}
