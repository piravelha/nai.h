#include "../../nai.h"

typedef struct {
    String_View message;
    String_View name;
} Tag;

typedef Array(Tag) Tags;

bool match_tag(Token_Stream *stream, Tag *tag)
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
    Tags tags = ts_match(Tags, lexer_to_ts(lexer), match_tag);

    String_Builder defs = {0};
    array_foreach(Tag, tag, &tags) {
        sb_printf(&defs, "const char *const _"SV_FMT"_tag = "SV_FMT";\n",
                  SV_ARG(tag->name), SV_ARG(tag->message));
    }

    write_file("gen/defs.c", defs);

    Cmd cmd = {0};
    cmd_append(&cmd, "cc", "main.c");
    cmd_append(&cmd, "-o", "build/main");
    cmd_run(cmd);

    return 0;
}


