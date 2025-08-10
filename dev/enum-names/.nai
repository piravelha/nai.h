#include "../../nai.h"
#include "../../nai_macro.h"


bool process_enums(Token_Stream *in, Token_Stream *out)
{
    size_t start = in->index;

    if (!ts_expect_id(in, "enum")) return false;
    if (!ts_expect_type(in, '{')) return false;

    String names = {0};

    while (ts_valid(in)) {
        if (ts_expect_type(in, '}')) break;
        
        String_View name = ts_tok(in).value;
        if (!ts_expect_type(in, TT_ID)) return false;

        if (!ts_consume_until(in, ',')) {
            if (ts_consume_until(in, '}')) break;
            return false;
        }

        str_appendf(&names, "    ["SV_FMT"] = \""SV_FMT"\",\n", SV_ARG(name), SV_ARG(name));
    }

    String_View enum_name = ts_tok(in).value;
    if (!ts_expect_type(in, TT_ID)) return false;
    if (!ts_expect_type(in, ';')) return false;

    String result = {0};

    str_appendf(&result, "const char *_names_"SV_FMT"[] = {\n", SV_ARG(enum_name));
    str_appendf(&result, SV_FMT, SV_ARG(names));
    str_append(&result, "};");

    ts_extend(out, ts_start_from(in, start));
    ts_extend(out, ts_fmt(SV_FMT, SV_ARG(result)));

    return true;
}

bool process_enum_name(Token_Stream *in, Token_Stream *out)
{
    Token_Stream content = {0};
    if (!ts_balanced(in, &content, "[", "]")) return false;

    String_View type = ts_tok(&content).value;
    if (!ts_expect_type(&content, TT_ID)) return false;
    if (!ts_expect_value(&content, ":")) return false;

    *out = ts_fmt("_names_"SV_FMT"["SV_FMT"]", SV_ARG(type), SV_ARG(ts_render(&content)));
    return true;
}

int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    Token_Stream out = lex_file("main.c");
    out = ts_replace(out, process_enums);
    out = ts_replace(out, process_enum_name);

    write_file(".generated.c", out.content);

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", ".generated.c", "-o", "main");
    int exit_code = cmd_run(cmd, .debug = true);

    return exit_code;
}
