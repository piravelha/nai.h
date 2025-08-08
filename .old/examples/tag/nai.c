#include "../../nai.h"

typedef struct {
    String_View message;
    String_View name;
} Tag;

typedef Array(Tag) Tags;

bool parse_tag(Token_Stream *stream, Tag *tag)
{
    if (!ts_expect_id(stream, "TAG")) return false;
    if (!ts_expect_type(stream, '(')) return false;

    String_View message = ts_tok(stream).value;
    if (!ts_expect_type(stream, TT_STRING)) return false;

    if (!ts_expect_type(stream, ')')) return false;

    for (size_t i = stream->index - 4; i >= 0; --i) {
        Token token = stream->tokens.items[i];

        if (token.type != '(') continue;
        String_View name = stream->tokens.items[i - 1].value;

        tag->message = message;
        tag->name = name;

        return true;
    }

    return false;
}


int main(int argc, char **argv)
{
    REBUILD(argc, argv);

    Lexer lexer = lex_file("main.c");
    Tags tags = ts_match(Tags, lexer_to_ts(lexer), parse_tag);

    String_Builder defs = {0};

    array_foreach(Tag, tag, &tags) {
        sb_print_many(&defs,
            {  S = "const char *const _" },
            { SV = tag->name },
            {  S = "_tag = " },
            { SV = tag->message },
            {  S = ";\n" });
    }

    write_file("generated/defs.c", defs);

    add_build_file(.name = "main.c", .out = "build/main");
    run_build();

    return 0;
}


