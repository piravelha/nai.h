#define NAI_FLAGS "-I../../"
#include "nai_macro.h"


bool process_enums(Token_Stream *in, Token_Stream *out)
{
    size_t start = in->index;

    TRY(ts_expect_value(in, "typedef"));
    TRY(ts_expect_value(in, "enum"));
    TRY(ts_expect_type(in, '{'));

    String names = {0};

    while (ts_valid(*in)) {
        if (ts_expect_type(in, '}')) break;
        
        String_View name = ts_value(*in);

        TRY(ts_expect_type(in, TT_ID));
        TRY(ts_consume_until(in, ','));

        str_appendf(&names, "    case %.*s: return \"%.*s\";\n", SV(name), SV(name));
    }

    String_View enum_name = ts_value(*in);
    TRY(ts_expect_type(in, TT_ID));
    TRY(ts_expect_type(in, ';'));

    String result = {0};

    str_appendf(&result, "const char *_names_%.*s(%.*s _)\n", SV(enum_name), SV(enum_name));
    str_append(&result , "{\n");
    str_append(&result , "    switch (_) {\n");
    str_appendf(&result, "%.*s", SV(names));
    str_append(&result , "    }\n");
    str_append(&result, "}");

    ts_extend(out, ts_start_from(in, start));
    ts_extend(out, ts_fmt("%.*s", SV(result)));

    return true;
}

bool process_enum_name(Token_Stream *in, Token_Stream *out)
{
    TRY(ts_expect_value(in, "nameof"));

    Token_Stream content = {0};
    TRY(ts_balanced(in, &content, "(", ")"));

    String_View type = ts_value(content);
    TRY(ts_expect_type(&content, TT_ID));
    TRY(ts_expect_type(&content, ','));
    String value = ts_render(content);

    *out = ts_fmt("_names_%.*s(%.*s)", SV(type), SV(value));
    return true;
}

int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    Token_Stream out = lex_file("main.c");
    out = ts_replace(out, process_enums);
    out = ts_replace(out, process_enum_name);

    write_file(".generated.c", out.content);
    format_file(".generated.c");

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", ".generated.c", "-o", "main");

    int exit_code = cmd_run(cmd, .debug = true);
    return exit_code;
}

