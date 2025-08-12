/* MIT License

Copyright (c) 2025 piraloco

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */



#ifndef NAI_MACRO_H
#define NAI_MACRO_H

#include "nai.h"


typedef enum {
    NAI_TT_INVALID = 0,
    NAI_TT_ID = 256,
    NAI_TT_INTEGER,
    NAI_TT_STRING,
    NAI_TT_CHARACTER,
    NAI_TT_INCLUDE_STRING,
    NAI_TT_PREPROC,
    NAI_TT_BITWISE_AND,
    NAI_TT_BITWISE_OR,
    NAI_TT_LESS_EQUAL,
    NAI_TT_GREATER_EQUAL,
    NAI_TT_DOUBLE_EQUAL,
    NAI_TT_NOT_EQUAL,
    NAI_TT_ARROW,
    NAI_TT_COMMENT,
} Nai_Token_Type;



static const char *const nai_tt_debug_names[];



typedef struct {
    Nai_Token_Type type;
    Nai_String_View value;
    size_t position;
} Nai_Token;



typedef Nai_Array(Nai_Token) Nai_Tokens;


typedef struct {
    const char *file_name;
    Nai_String_View content;
    size_t position;
    size_t start;

    Nai_Tokens tokens;
} Nai_Lexer;


typedef struct {
    Nai_Tokens tokens;
    size_t index;
    Nai_String_View content;
    Nai_Token_Type expected_type;
    Nai_String_View expected_value;
    Nai_String_View got_value;
} Nai_Token_Stream;


#define TRY(...) if(!(__VA_ARGS__)) return false


Nai_Token_Stream nai_lex_sv(const char *file_name, Nai_String_View content);
Nai_Token_Stream nai_lex_file(const char *file_name);



typedef Nai_Array(void) Nai_Generic_Array;


bool nai_ts_expect_type(Nai_Token_Stream *stream, Nai_Token_Type type);
bool nai_ts_expect_value(Nai_Token_Stream *stream, const char *value);
bool nai_ts_consume_until(Nai_Token_Stream *stream, Nai_Token_Type value);
Nai_String_View nai_ts_value(Nai_Token_Stream stream);
Nai_Token_Stream nai_lexer_to_ts(Nai_Lexer lexer);
bool nai_ts_balanced(Nai_Token_Stream *stream, Nai_Token_Stream *result, const char *left, const char *right);
const char *nai_ts_get_error(Nai_Token_Stream stream);
bool nai_ts_valid(Nai_Token_Stream stream);

Nai_Token_Stream nai_ts_replace(Nai_Token_Stream stream, bool (*transform)(Nai_Token_Stream *in, Nai_Token_Stream *out));

Nai_String nai_ts_render(Nai_Token_Stream ts);

#define nai_ts_fmt(fmt, ...) nai_lex_sv("<temp>", nai_sv_from_cstr(nai_temp_sprintf(fmt, ##__VA_ARGS__)))
#define nai_ts_stringify(...) nai_lex_sv("<temp>", nai_sv_from_cstr(#__VA_ARGS__))


#ifndef NAI_HEADER_ONLY


static const char *const nai_tt_debug_names[] = {
    [NAI_TT_ID] = "IDENTIFIER",
    [NAI_TT_INTEGER] = "INTEGER",
    [NAI_TT_STRING] = "STRING",
    [NAI_TT_CHARACTER] = "CHARACTER",
    [NAI_TT_INCLUDE_STRING] = "INCLUDE_STRING",
    [NAI_TT_PREPROC] = "PREPROC",
    [NAI_TT_BITWISE_AND] = "&&",
    [NAI_TT_BITWISE_OR] = "||",
    [NAI_TT_LESS_EQUAL] = "<=",
    [NAI_TT_GREATER_EQUAL] = ">=",
    [NAI_TT_DOUBLE_EQUAL] = "==",
    [NAI_TT_NOT_EQUAL] = "!=",
    [NAI_TT_ARROW] = "->",
    [NAI_TT_COMMENT] = "COMMENT",
};


bool nai_token_is_id(Nai_Token token, const char *id)
{
    return token.type == NAI_TT_ID && !strncmp(token.value.data, id, strlen(id));
}




Nai_String nai_tokens_render(Nai_Tokens tokens)
{
    Nai_String rendered = {0};

    for (size_t i = 0; i < tokens.count; ++i) {
        if (i > 0) nai_str_append(&rendered, " ");
        nai_str_appendf(&rendered, NAI_SV_FMT, NAI_SV_ARG(tokens.items[i].value));
    }

    return rendered;
}

Nai_String nai_ts_render(Nai_Token_Stream ts)
{
    Nai_String rendered = {0};

    for (size_t i = ts.index; i < ts.tokens.count; ++i) {
        nai_str_appendf(&rendered, NAI_SV_FMT" ", NAI_SV_ARG(ts.tokens.items[i].value));
    }

    return rendered;
}

char nai_lexer_ch(Nai_Lexer *lexer)
{
    if (lexer->position >= lexer->content.count) {
        return 0;
    }

    return lexer->content.data[lexer->position];
}


void nai_lexer_trim(Nai_Lexer *lexer)
{
    while (isspace(nai_lexer_ch(lexer))) {
        ++lexer->position;
    }
}


void nai_lexer_append(Nai_Lexer *lexer, Nai_Token_Type type)
{
    nai_array_append(&lexer->tokens, (Nai_Token) {
        .type = type,
        .value = {
            .data = lexer->content.data + lexer->start,
            .count = lexer->position - lexer->start,
        },
        .position = lexer->start,
    });
}

void nai_lex_id(Nai_Lexer *lexer)
{
    ++lexer->position;

    while (isalnum(nai_lexer_ch(lexer)) || nai_lexer_ch(lexer) == '_') {
        ++lexer->position;
    }

    nai_lexer_append(lexer, NAI_TT_ID);
}

void nai_lex_preproc(Nai_Lexer *lexer)
{
    ++lexer->position;

    if (isalpha(nai_lexer_ch(lexer)) || nai_lexer_ch(lexer) == '_') {
        ++lexer->position;

        while (isalnum(nai_lexer_ch(lexer)) || nai_lexer_ch(lexer) == '_') {
            ++lexer->position;
        }

        nai_lexer_append(lexer, NAI_TT_PREPROC);
    } else {
        nai_lexer_append(lexer, NAI_TT_INVALID);
    }
}

void nai_lex_integer(Nai_Lexer *lexer)
{
    ++lexer->position;

    while (isdigit(nai_lexer_ch(lexer))) {
        ++lexer->position;
    }

    nai_lexer_append(lexer, NAI_TT_INTEGER);
}


void nai_lex_string(Nai_Lexer *lexer) {
    ++lexer->position;

    while (nai_lexer_ch(lexer) && nai_lexer_ch(lexer) != '"') {
        if (nai_lexer_ch(lexer) == '\\') {
            ++lexer->position;
        }

        ++lexer->position;
    }

    if (!nai_lexer_ch(lexer)) {
        nai_lexer_append(lexer, NAI_TT_INVALID); // TODO: add info that this is unterminated string
    } else {
        ++lexer->position;
        nai_lexer_append(lexer, NAI_TT_STRING);
    }
}

void nai_lex_include_string(Nai_Lexer *lexer) {
    ++lexer->position;

    while (nai_lexer_ch(lexer) && nai_lexer_ch(lexer) != '>') {
        if (nai_lexer_ch(lexer) == '\\') {
            ++lexer->position;
        }

        ++lexer->position;
    }

    if (!nai_lexer_ch(lexer)) {
        nai_lexer_append(lexer, NAI_TT_INVALID); // TODO: add info that this is unterminated string
    } else {
        ++lexer->position;
        nai_lexer_append(lexer, NAI_TT_INCLUDE_STRING);
    }
}

void nai_lex_character(Nai_Lexer *lexer)
{
    ++lexer->position;

    if (nai_lexer_ch(lexer) == '\'') {
        nai_lexer_append(lexer, NAI_TT_INVALID);
        return;
    }

    if (nai_lexer_ch(lexer) == '\\') {
        ++lexer->position;
    } 
    
    ++lexer->position;

    if (nai_lexer_ch(lexer) != '\'') {
        nai_lexer_append(lexer, NAI_TT_INVALID);
    } else {
        ++lexer->position;
        nai_lexer_append(lexer, NAI_TT_CHARACTER);
    }
}

void nai_lex_comment(Nai_Lexer *lexer)
{
    lexer->position += 2;

    while (nai_lexer_ch(lexer) && nai_lexer_ch(lexer) != '\n') {
        ++lexer->position;
    }

    nai_lexer_append(lexer, NAI_TT_COMMENT);
}



Nai_Token_Stream nai_lex_sv(const char *file_name, Nai_String_View content)
{
    Nai_Lexer lexer = {0};
    lexer.content = content;
    lexer.file_name = file_name;

    while (nai_lexer_ch(&lexer)) {
        nai_lexer_trim(&lexer);

        if (!nai_lexer_ch(&lexer)) break;
        
        lexer.start = lexer.position;

        if (nai_lexer_ch(&lexer) == '#') {
            nai_lex_preproc(&lexer);
        } else if (isalpha(nai_lexer_ch(&lexer)) || nai_lexer_ch(&lexer) == '_') {
            nai_lex_id(&lexer);
        } else if (isdigit(nai_lexer_ch(&lexer))) {
            nai_lex_integer(&lexer);
        } else if (nai_lexer_ch(&lexer) == '"') {
            nai_lex_string(&lexer);
        } else if (nai_lexer_ch(&lexer) == '<' &&
                lexer.tokens.count > 0 &&
                lexer.tokens.items[lexer.tokens.count - 1].type == NAI_TT_PREPROC)
        {
            nai_lex_include_string(&lexer);
        } else if (nai_lexer_ch(&lexer) == '\'') {
            nai_lex_character(&lexer);
        } else if (!strncmp(&content.data[lexer.position], "&&", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_BITWISE_AND);
        } else if (!strncmp(&content.data[lexer.position], "||", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_BITWISE_OR);
        } else if (!strncmp(&content.data[lexer.position], "<=", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_LESS_EQUAL);
        } else if (!strncmp(&content.data[lexer.position], ">=", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_GREATER_EQUAL);
        } else if (!strncmp(&content.data[lexer.position], "==", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_DOUBLE_EQUAL);
        } else if (!strncmp(&content.data[lexer.position], "!=", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_NOT_EQUAL);
        } else if (!strncmp(&content.data[lexer.position], "->", 2)) {
            lexer.position += 2;
            nai_lexer_append(&lexer, NAI_TT_ARROW);
        } else if (!strncmp(&content.data[lexer.position], "//", 2)) {
            nai_lex_comment(&lexer);
        } else if (nai_lexer_ch(&lexer) == '(' ||
               nai_lexer_ch(&lexer) == ')' ||
               nai_lexer_ch(&lexer) == '{' ||
               nai_lexer_ch(&lexer) == '}' ||
               nai_lexer_ch(&lexer) == '[' ||
               nai_lexer_ch(&lexer) == ']' ||
               nai_lexer_ch(&lexer) == ';' ||
               nai_lexer_ch(&lexer) == '*' ||
               nai_lexer_ch(&lexer) == '=' ||
               nai_lexer_ch(&lexer) == '.' ||
               nai_lexer_ch(&lexer) == ',' ||
               nai_lexer_ch(&lexer) == '-' ||
               nai_lexer_ch(&lexer) == '+' ||
               nai_lexer_ch(&lexer) == '!' ||
               nai_lexer_ch(&lexer) == '<' ||
               nai_lexer_ch(&lexer) == '>' ||
               nai_lexer_ch(&lexer) == '&' ||
               nai_lexer_ch(&lexer) == '/' ||
               nai_lexer_ch(&lexer) == '\\' ||
               nai_lexer_ch(&lexer) == '|')
        {
            ++lexer.position;
            nai_lexer_append(&lexer, content.data[lexer.position - 1]);
        } else {
            ++lexer.position;
            nai_lexer_append(&lexer, 0);
        }
    }

    return nai_lexer_to_ts(lexer);
}



Nai_Token_Stream nai_lex_file(const char *file_name)
{
    Nai_String content = {0};

    nai_read_file(file_name, &content);
    return nai_lex_sv(file_name, nai_str_to_sv(content));
}


bool nai_ts_expect_value(Nai_Token_Stream *stream, const char *value)
{
    if (!nai_ts_valid(*stream)) return false;
    Nai_Token token = stream->tokens.items[stream->index];
    if (!token.value.data) return false;
    
    if (!strncmp(token.value.data, value, strlen(value))) {
        ++stream->index;
        return true;
    } else {
        stream->expected_value = nai_sv_from_cstr(value);
        stream->got_value = token.value;
        return false;
    }
}


bool nai_ts_expect_type(Nai_Token_Stream *stream, Nai_Token_Type type)
{
    if (!nai_ts_valid(*stream)) return false;
    Nai_Token token = stream->tokens.items[stream->index];

    if (token.type == type) {
        ++stream->index;
        return true;
    } else {
        stream->expected_type = type;
        stream->got_value = token.value;
        return false;
    }
}

Nai_String_View nai_ts_value(Nai_Token_Stream stream)
{
    if (stream.index < stream.tokens.count) {
        return stream.tokens.items[stream.index].value;
    } else {
        return (Nai_String_View) {0};
    }
}

Nai_Token_Stream nai_lexer_to_ts(Nai_Lexer lexer)
{
    return (Nai_Token_Stream) {
        .tokens = lexer.tokens,
        .content = lexer.content,
    };
}

const char *nai_ts_get_error(Nai_Token_Stream stream)
{
    Nai_String message = {0};

    nai_str_append(&message, "ERROR: ");

    if (stream.expected_value.data) {
        nai_str_appendf(&message, "Expected '"NAI_SV_FMT"', but got '"NAI_SV_FMT"' instead\n", NAI_SV_ARG(stream.expected_value), NAI_SV_ARG(stream.got_value));
    } else {
        if (stream.expected_type <= 255) {
            nai_str_appendf(&message, "Expected '%c', but got '"NAI_SV_FMT"' instead\n", stream.expected_type, NAI_SV_ARG(stream.got_value));
        } else {
            nai_str_appendf(&message, "Expected '%s', but got '"NAI_SV_FMT"' instead\n", nai_tt_debug_names[stream.expected_type], NAI_SV_ARG(stream.got_value));
        }
    }

    nai_str_append_null(&message);

    return message.data;
}


bool nai_ts_balanced(Nai_Token_Stream *stream, Nai_Token_Stream *result, const char *left, const char *right)
{
    result->content = stream->content;
    result->index = 0;

    if (!nai_ts_expect_value(stream, left)) return false;

    int count = 1;

    while (stream->index < stream->tokens.count) {
        Nai_Token tok = stream->tokens.items[stream->index];

        if (nai_ts_expect_value(stream, left)) {
            ++count;
            nai_array_append(&result->tokens, tok);

            continue;
        }

        if (nai_ts_expect_value(stream, right)) {
            --count;

            if (!count) break;
            nai_array_append(&result->tokens, tok);

            continue;
        }

        if (!count) break;

        nai_array_append(&result->tokens, tok);
        ++stream->index;
    }

    return count == 0;
}




Nai_Token_Stream nai_ts_replace(Nai_Token_Stream stream, bool (*transform)(Nai_Token_Stream *, Nai_Token_Stream *))
{
    Nai_String content = {0};
    Nai_Token_Stream out = {0};

    size_t last_pos = 0;

    while (stream.index < stream.tokens.count) {
        Nai_Token_Stream copy = stream;
        Nai_Token_Stream result = {0};

        if (transform(&copy, &result)) {
            assert(copy.index > stream.index && "Infinite loop detected");

            size_t pos = stream.tokens.items[stream.index].position;
            size_t size = pos - last_pos;

            nai_str_reserve(&content, content.count + size);
            memcpy(content.data + content.count, stream.content.data + last_pos, size);
            content.count += size;

           nai_array_foreach(Nai_Token, tok, &result.tokens) {
                tok->position = content.count;

                nai_str_appendf(&content, NAI_SV_FMT" ", NAI_SV_ARG(tok->value));
                nai_array_append(&out.tokens, *tok);
            }

            if (copy.index < stream.tokens.count) {
                last_pos = stream.tokens.items[copy.index].position;
            }
            stream = copy;

            continue;
        } else {
            size_t pos = stream.tokens.items[stream.index].position;
            size_t size = pos - last_pos;

            nai_str_reserve(&content, content.count + size);
            memcpy(content.data + content.count, stream.content.data + last_pos, size);
            content.count += size;

            Nai_Token tok = stream.tokens.items[stream.index];
            tok.position = content.count;

            nai_array_append(&out.tokens, tok);
            last_pos = pos;
        }

        ++stream.index;
    }

    size_t size = stream.content.count - last_pos;

    nai_str_reserve(&content, content.count + size);
    memcpy(content.data + content.count, stream.content.data + last_pos, size);

    content.count += size;

    out.content = nai_str_to_sv(content);
    return out;
}

void nai_ts_write_file(const char *file_name, Nai_Token_Stream ts)
{
    nai_write_file(file_name, ts.content);
}

bool nai_ts_valid(Nai_Token_Stream ts)
{
    return ts.index < ts.tokens.count;
}

bool nai_ts_consume_until(Nai_Token_Stream *ts, Nai_Token_Type value)
{
    Nai_Token_Stream copy = *ts;

    while (nai_ts_valid(copy) && copy.tokens.items[copy.index].type != value) {
        ++copy.index;
    }

    if (!nai_ts_valid(copy)) return false;

    ++copy.index;
    *ts = copy;
    return true;
}

Nai_Token_Stream nai_ts_start_from(Nai_Token_Stream *ts, size_t from)
{
    Nai_Token_Stream result = {0};

    for (size_t i = from; i < ts->index; ++i) {
       nai_array_append(&result.tokens, ts->tokens.items[i]);
    }

    return result;
}

void nai_ts_extend(Nai_Token_Stream *ts, Nai_Token_Stream other)
{
    nai_array_foreach(Nai_Token, tok, &other.tokens) {
        nai_array_append(&ts->tokens, *tok);
    }
}

void nai_format_file(const char *path)
{
    Cmd cmd = {0};
    cmd_append(&cmd, "clang-format", "-style=\"{BasedOnStyle: LLVM, IndentWidth: 4}\"", "-i", path);
    cmd_run(cmd);
}

#endif // NAI_HEADER_ONLY


#ifndef NAI_FORCE_PREFIX
#define Token_Type Nai_Token_Type
#define TT_INVALID NAI_TT_INVALID
#define TT_ID NAI_TT_ID
#define TT_INTEGER NAI_TT_INTEGER
#define TT_STRING NAI_TT_STRING
#define TT_CHARACTER NAI_TT_CHARACTER
#define TT_INCLUDE_STRING NAI_TT_INCLUDE_STRING
#define TT_PREPROC NAI_TT_PREPROC
#define TT_BITWISE_AND NAI_TT_BITWISE_AND
#define TT_BITWISE_OR NAI_TT_BITWISE_OR
#define TT_LESS_EQUAL NAI_TT_LESS_EQUAL
#define TT_GREATER_EQUAL NAI_TT_GREATER_EQUAL
#define TT_DOUBLE_EQUAL NAI_TT_DOUBLE_EQUAL
#define TT_NOT_EQUAL NAI_TT_NOT_EQUAL
#define TT_ARROW NAI_TT_ARROW
#define TT_COMMENT NAI_TT_COMMENT
#define Token Nai_Token
#define Tokens Nai_Tokens
#define Lexer Nai_Lexer
#define lex_sv nai_lex_sv
#define lex_file nai_lex_file
#define Token_Stream Nai_Token_Stream
#define ts_expect_value nai_ts_expect_value
#define ts_expect_type nai_ts_expect_type
#define ts_value nai_ts_value
#define ts_get_error nai_ts_get_error
#define ts_balanced nai_ts_balanced
#define ts_consume_until nai_ts_consume_until
#define ts_replace nai_ts_replace
#define ts_fmt nai_ts_fmt
#define ts_stringify nai_ts_stringify
#define ts_write_file nai_ts_write_file
#define ts_valid nai_ts_valid
#define ts_render nai_ts_render
#define ts_start_from nai_ts_start_from
#define ts_extend nai_ts_extend
#define format_file nai_format_file
#define lexer_to_ts nai_lexer_to_ts

#endif // NAI_FORCE_PREFIX



#endif // NAI_MACRO_H
