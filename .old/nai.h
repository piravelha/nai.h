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


#ifndef NAI_H
#define NAI_H

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>



#ifndef NAI_FLAGS
#define NAI_FLAGS ""
#endif


#define NAI_TODO(msg) assert(0 && "TODO: "msg)


#ifndef NAI_GLOBAL_ARENA_SIZE
#define NAI_GLOBAL_ARENA_SIZE 16 * 1024 * 1024
#endif

static struct {
    void *memory;
    void *alloc_pointer;
} nai_global_arena;

void nai_init_global_arena(void);
void *nai_global_arena_alloc(size_t alloc_size);



#ifndef NAI_SB_INIT_CAPACITY
#define NAI_SB_INIT_CAPACITY 64
#endif

typedef struct {
    char *data;
    size_t count;
    size_t capacity;
} Nai_String_Builder;

void nai_sb_reserve(Nai_String_Builder *sb, size_t size);
__attribute__((format(printf, 2, 3)))
void nai_sb_printf(Nai_String_Builder *sb, const char *fmt, ...);
__attribute__((format(printf, 1, 2)))
const char *nai_temp_sprintf(const char *fmt, ...);
void nai_sb_print_null(Nai_String_Builder *sb);

typedef struct {
    const char *data;
    size_t count;
} Nai_String_View;

void nai_sb_print_sv(Nai_String_Builder *sb, Nai_String_View sv);

#define NAI_SV_FMT "%.*s"
#define NAI_SV_ARG(sv) (int) (sv).count, (sv).data

Nai_String_View nai_sb_to_sv(Nai_String_Builder sb);
Nai_String_Builder nai_sv_to_sb(Nai_String_View sv);
bool nai_sv_equals(Nai_String_View sv, const char *cstr);
Nai_String_View nai_sv_chop_space(Nai_String_View *sv);
Nai_String_View nai_sv_chop(Nai_String_View *sv, char delim);
Nai_String_View nai_sv_chop_right(Nai_String_View sv, char delim);
Nai_String_View nai_sv_from_cstr(const char *cstr);



#ifndef NAI_ARRAY_INIT_CAPACITY
#define NAI_ARRAY_INIT_CAPACITY 16
#endif
 
#define Nai_Array(type) struct { \
    type *items; \
    size_t count; \
    size_t capacity; \
}

#define nai_array_append(array, ...) do { \
    if ((array)->count >= (array)->capacity) { \
        size_t _new_capacity = (array)->capacity * 2; \
        if (_new_capacity == 0) { \
            _new_capacity = NAI_ARRAY_INIT_CAPACITY; \
        } \
        void *_new_data = nai_global_arena_alloc(_new_capacity * sizeof((array)->items[0])); \
        memcpy(_new_data, (array)->items, (array)->count * sizeof((array)->items[0])); \
        (array)->items = _new_data; \
        (array)->capacity = _new_capacity; \
    } \
    (array)->items[(array)->count++] = (__VA_ARGS__); \
} while (0)

#define nai_array_append_void(array, size, ...) do { \
    if ((array)->count >= (array)->capacity) { \
        size_t _new_capacity = (array)->capacity * 2; \
        if (_new_capacity == 0) { \
            _new_capacity = NAI_ARRAY_INIT_CAPACITY; \
        } \
        void *_new_data = nai_global_arena_alloc(_new_capacity * size); \
        memcpy(_new_data, (array)->items, (array)->count * size); \
        (array)->items = _new_data; \
        (array)->capacity = _new_capacity; \
    } \
    memcpy((array)->items + (array)->count * size, (__VA_ARGS__), size); \
    (array)->count++; \
} while (0)


#define nai_array_foreach(type, x, array) for (type *x = (array)->items; x != (array)->items + (array)->count; ++x)



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



Nai_Lexer nai_lex(const char *file_name, Nai_String_View content);
Nai_Lexer nai_lex_file(const char *file_name);


bool nai_read_file(const char *file_path, Nai_String_Builder *output);
bool nai_write_file(const char *file_path, Nai_String_Builder content);



/*
typedef Nai_Array(const char *) Nai_Cmd;

void nai_cmd_append_(Nai_Cmd *cmd, ...);
#define nai_cmd_append(cmd, ...) nai_cmd_append_((cmd), __VA_ARGS__, NULL)
void nai_cmd_run(Nai_Cmd cmd);
*/



typedef struct {
    const char *name;
    const char *out;
} Nai_Add_Build_File_Params;

typedef Nai_Array(const char *) Nai_Cmd;

static Nai_Cmd nai_build_cmd;

void nai_add_build_file_(Nai_Add_Build_File_Params ps);
#define nai_add_build_file(...) nai_add_build_file_((Nai_Add_Build_File_Params){ __VA_ARGS__ })

void nai_run_build(void);


char *nai_arg_shift(int *argc, char ***argv);



typedef struct {
    Nai_Tokens tokens;
    size_t index;
    Nai_String_View content;
    Nai_Token_Type expected_type;
    Nai_String_View expected_value;
    Nai_String_View got_value;
} Nai_Token_Stream;


typedef Nai_Array(void) Nai_Generic_Array;


bool nai_ts_expect_id(Nai_Token_Stream *stream, const char *id);
bool nai_ts_expect_value(Nai_Token_Stream *stream, const char *value);
bool nai_ts_expect_type(Nai_Token_Stream *stream, Nai_Token_Type type);
Nai_Token nai_ts_tok(Nai_Token_Stream *stream);
Nai_Token_Stream nai_lexer_to_ts(Nai_Lexer lexer);
bool  nai_ts_balanced(Nai_Token_Stream *stream, Nai_Token_Stream *result, const char *left, const char *right);
const char *nai_ts_get_error(Nai_Token_Stream stream);
Nai_Generic_Array nai_ts_match_(Nai_Token_Stream stream, bool (*transform)(Nai_Token_Stream *, void *), size_t);

#define nai_ts_match(type, stream, func) (sizeof(func(&(Nai_Token_Stream){0}, (type){0}.items)), \
        ((union { Nai_Generic_Array from; type to; }){ .from = nai_ts_match_(stream, (void *)func, sizeof(((type){0}).items[0])) }).to)


void nai_recompile_metaprogram(int argc, char **argv, const char *file_name);
void nai_rebuild_metaprogram_if_needed(int argc, char **argv, const char *file_name);


#define NAI_REBUILD(argc, argv) nai_rebuild_metaprogram_if_needed(argc, argv, __FILE__)





#ifndef NAI_HEADER_ONLY


void nai_init_global_arena(void)
{
    void *memory = malloc(NAI_GLOBAL_ARENA_SIZE);
    assert(memory && "Out of memory");

    nai_global_arena.memory = memory;
    nai_global_arena.alloc_pointer = memory;
}


void *nai_global_arena_alloc(size_t alloc_size)
{
    if (!nai_global_arena.memory) {
        nai_init_global_arena();
    }

    assert(nai_global_arena.alloc_pointer - nai_global_arena.memory + alloc_size < NAI_GLOBAL_ARENA_SIZE);

    void *pointer = nai_global_arena.alloc_pointer;
    nai_global_arena.alloc_pointer += alloc_size;
    return pointer;
}


void nai_sb_reserve(Nai_String_Builder *sb, size_t size)
{
    if (size > sb->capacity) {
        size_t new_capacity = size * 2;
        char *new_data = nai_global_arena_alloc(new_capacity);
        memcpy(new_data, sb->data, sb->count);

        sb->data = new_data;
        sb->capacity = new_capacity;
    }
}


// TODO: currently only for gnu/clang
__attribute__((format(printf, 2, 3)))
void nai_sb_printf(Nai_String_Builder *sb, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t formatted_length = vsnprintf(NULL, 0, fmt, args);
    nai_sb_reserve(sb, sb->count + formatted_length);

    vsprintf(sb->data + sb->count, fmt, args);
    sb->count += formatted_length;
    
    va_end(args);
}

void nai_sb_print_sv(Nai_String_Builder *sb, Nai_String_View sv)
{
    nai_sb_printf(sb, NAI_SV_FMT, NAI_SV_ARG(sv));
}

void nai_sb_print_null(Nai_String_Builder *sb)
{
    nai_sb_reserve(sb, sb->count + 1);
    sb->data[sb->count++] = 0;
}



// TODO: currently only for gnu/clang
__attribute__((format(printf, 1, 2)))
const char *nai_temp_sprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t formatted_length = vsnprintf(NULL, 0, fmt, args);

    Nai_String_Builder sb = {0};
    nai_sb_reserve(&sb, formatted_length);

    vsprintf(sb.data, fmt, args);
    sb.count = formatted_length;

    nai_sb_print_null(&sb);
    
    va_end(args);

    return sb.data;
}


Nai_String_View nai_sb_to_sv(Nai_String_Builder sb)
{
    return (Nai_String_View) {
        .data = sb.data,
        .count = sb.count,
    };
}


Nai_String_Builder nai_sv_to_sb(Nai_String_View sv)
{
    Nai_String_Builder sb = {0};
    nai_sb_printf(&sb, NAI_SV_FMT, NAI_SV_ARG(sv));

    return sb;
}


bool nai_sv_equals(Nai_String_View sv, const char *cstr)
{
    return !strncmp(sv.data, cstr, sv.count);
}


Nai_String_View nai_sv_chop_space(Nai_String_View *sv)
{
    const char *data = sv->data;
    
    while (sv->count) {
        if (isspace(sv->data[0])) {
            Nai_String_View chopped = {
                .data = data,
                .count = sv->data - data,
            };

            ++sv->data;
            --sv->count;

            return chopped;
        }

        ++sv->data;
        --sv->count;
    }

    return *sv;
}


Nai_String_View nai_sv_chop(Nai_String_View *sv, char delim)
{
    const char *data = sv->data;
    
    while (sv->count) {
        if (sv->data[0] == delim) {
            Nai_String_View chopped = {
                .data = data,
                .count = sv->data - data,
            };

            ++sv->data;
            --sv->count;

            return chopped;
        }

        ++sv->data;
        --sv->count;
    }

    return *sv;
}


Nai_String_View nai_sv_chop_right(Nai_String_View sv, char delim)
{
    for (size_t i = sv.count - 1; i >= 0; --i) {
        if (sv.data[i] == delim) {
            return (Nai_String_View) {
                .data = sv.data,
                .count = i,
            };
        }
    }

    return sv;
}


Nai_String_View nai_sv_from_cstr(const char *cstr)
{
    return (Nai_String_View) {
        .data = cstr,
        .count = strlen(cstr),
    };
}



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




Nai_String_Builder nai_tokens_render(Nai_Tokens tokens)
{
    Nai_String_Builder rendered = {0};

    for (size_t i = 0; i < tokens.count; ++i) {
        if (i > 0) nai_sb_printf(&rendered, " ");
        nai_sb_printf(&rendered, NAI_SV_FMT, NAI_SV_ARG(tokens.items[i].value));
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



Nai_Lexer nai_lex(const char *file_name, Nai_String_View content)
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
                lexer.position > 0 &&
                nai_sv_equals(lexer.tokens.items[lexer.position - 1].value, "#include"))
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

    return lexer;
}



bool nai_read_file(const char *file_path, Nai_String_Builder *output)
{
    FILE *file = fopen(file_path, "rb");
    if (!file) return false;

    if (fseek(file, 0, SEEK_END) != 0) return false;
    size_t file_size = ftell(file);
    if (file_size < 0) return false;

    rewind(file);

    nai_sb_reserve(output, output->count + file_size);

    fread(output->data + output->count, 1, file_size, file);

    output->count += file_size;

    fclose(file);

    return true;
}


bool nai_write_file(const char *file_path, Nai_String_Builder content)
{
    FILE *file = fopen(file_path, "w+");
    if (!file) return false;

    fwrite(content.data, 1, content.count, file);
    fclose(file);

    return true;
}


Nai_Lexer nai_lex_file(const char *file_name)
{
    Nai_String_Builder content = {0};

    nai_read_file(file_name, &content);
    return nai_lex(file_name, nai_sb_to_sv(content));
}


/*
void nai_cmd_append_(Nai_Cmd *cmd, ...)
{
    va_list args;
    va_start(args, cmd);

    const char *arg;
    while ((arg = va_arg(args, const char *))) {
        nai_array_append(cmd, arg);
    }

    va_end(args);
}


void nai_cmd_run(Nai_Cmd cmd)
{
    Nai_String_Builder builder = {0};

    for (size_t i = 0; i < cmd.count; ++i) {
        nai_sb_printf(&builder, "%s ", cmd.items[i]);
    }

    nai_sb_print_null(&builder);
    system(builder.data);
}
*/

void nai_recompile_metaprogram(int argc, char **argv, const char *file_name)
{

    system(nai_temp_sprintf("cc %s -o %s " NAI_FLAGS, file_name, argv[0]));

    FILE *metaprogram = fopen(".nai", "w+");
    assert(metaprogram);

    Nai_String_Builder new_content = {0};
    assert(nai_read_file(file_name, &new_content));
    fwrite(new_content.data, 1, new_content.count, metaprogram);

    fclose(metaprogram);

    Nai_String_Builder run = {0};
    
    for (size_t i = 0; i < argc; ++i) {
        nai_sb_printf(&run, " %s", argv[i]);
    }
    
    system(run.data);

    exit(0);
}


void nai_rebuild_metaprogram_if_needed(int argc, char **argv, const char *file_name)
{
    Nai_String_Builder file_contents = {0};
    assert(nai_read_file(file_name, &file_contents));

    nai_sb_print_null(&file_contents);

    Nai_String_Builder metaprogram_contents = {0};
    if (!nai_read_file(".nai", &metaprogram_contents)) {
        nai_recompile_metaprogram(argc, argv, file_name);
    }
    nai_sb_print_null(&metaprogram_contents);

    if (strcmp(file_contents.data, metaprogram_contents.data) != 0) {
        nai_recompile_metaprogram(argc, argv, file_name);
    }
}


char *nai_arg_shift(int *argc, char ***argv)
{
    char *arg = (*argv)[*argc];
    --(*argc);
    return arg;
}


bool nai_ts_expect_id(Nai_Token_Stream *stream, const char *id)
{
    Nai_Token token = stream->tokens.items[stream->index];
    if (!token.value.data) return false;
    
    if (token.type == NAI_TT_ID &&
        !strncmp(token.value.data, id, strlen(id))) {
        ++stream->index;
        return true;
    } else {
        stream->expected_type = NAI_TT_ID;
        stream->expected_value = nai_sv_from_cstr(id);
        stream->got_value = token.value;
        return false;
    }
}

bool nai_ts_expect_value(Nai_Token_Stream *stream, const char *value)
{
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

Nai_Token nai_ts_tok(Nai_Token_Stream *stream)
{
    if (stream->index < stream->tokens.count) {
        return stream->tokens.items[stream->index];
    } else {
        return (Nai_Token) {0};
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
    Nai_String_Builder message = {0};

    nai_sb_printf(&message, "ERROR: ");

    if (stream.expected_value.data) {
        nai_sb_printf(&message, "Expected '"NAI_SV_FMT"', but got '"NAI_SV_FMT"' instead\n", NAI_SV_ARG(stream.expected_value), NAI_SV_ARG(stream.got_value));
    } else {
        if (stream.expected_type <= 255) {
            nai_sb_printf(&message, "Expected '%c', but got '"NAI_SV_FMT"' instead\n", stream.expected_type, NAI_SV_ARG(stream.got_value));
        } else {
            nai_sb_printf(&message, "Expected '%s', but got '"NAI_SV_FMT"' instead\n", nai_tt_debug_names[stream.expected_type], NAI_SV_ARG(stream.got_value));
        }
    }

    nai_sb_print_null(&message);

    return message.data;
}


bool nai_ts_balanced(Nai_Token_Stream *stream, Nai_Token_Stream *result, const char *left, const char *right)
{
    result->content = stream->content;
    result->index = 0;

    if (!nai_ts_expect_value(stream, left)) return false;

    int count = 1;

    while (stream->index < stream->tokens.count) {
        Nai_Token tok = nai_ts_tok(stream);

        if (nai_ts_expect_value(stream, left)) {
            ++count;
            nai_array_append(&result->tokens, tok);

            if (!count) break;
            continue;
        }

        if (nai_ts_expect_value(stream, right)) {
            --count;
            nai_array_append(&result->tokens, tok);

            if (!count) break;
            continue;
        }

        if (!count) break;


        nai_array_append(&result->tokens, tok);
        ++stream->index;
    }

    return count == 0;
}


Nai_Generic_Array nai_ts_match_(Nai_Token_Stream stream, bool (*transform)(Nai_Token_Stream *, void *), size_t size)
{
    Nai_Generic_Array results = {0};
    void *value = nai_global_arena_alloc(size);

    while (stream.index < stream.tokens.count) {
        Nai_Token_Stream copy = stream;

        if (transform(&copy, value)) {
            nai_array_append_void(&results, size, value);
            value = nai_global_arena_alloc(size);
            stream = copy;
            continue;
        }

        ++stream.index;
    }

    return results;
}


void nai_add_build_file_(Nai_Add_Build_File_Params ps)
{
    if (ps.out) {
        nai_array_append(&nai_build_cmd, nai_temp_sprintf("cc %s -o %s", ps.name, ps.out));
    } else {
        nai_array_append(&nai_build_cmd, nai_temp_sprintf("cc %s", ps.name));
    }
}

void nai_run_build(void)
{
    nai_array_foreach(const char *, cmd, &nai_build_cmd) {
        system(*cmd);
    }
}

#endif // NAI_HEADER_ONLY


#ifndef NAI_FORCE_PREFIX

#define TODO NAI_TODO
#define Arena Nai_Arena
#define global_arena nai_global_arena
#define init_global_arena nai_init_global_arena
#define global_arena_alloc nai_global_arena_alloc
#define String_Builder Nai_String_Builder
#define sb_reserve nai_sb_reserve
#define temp_sprintf nai_temp_sprintf
#define sb_printf nai_sb_printf
#define sb_print_sv nai_sb_print_sv
#define sb_print_null nai_sb_print_null
#define sb_print_many nai_sb_print_many
#define print_many nai_print_many
#define println_many nai_println_many
#define S NAI_S
#define SV NAI_SV
#define String_View Nai_String_View
#define SV_FMT NAI_SV_FMT
#define SV_ARG NAI_SV_ARG
#define sb_to_sv nai_sb_to_sv
#define sv_to_sb nai_sv_to_sb
#define sv_equals nai_sv_equals
#define sv_chop nai_sv_chop
#define sv_chop_space nai_sv_chop_space
#define sv_chop_right nai_sv_chop_right
#define sv_from_cstr nai_sv_from_cstr
#define Array Nai_Array
#define array_append nai_array_append
#define array_foreach nai_array_foreach
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
#define lex_file nai_lex_file
#define read_file nai_read_file
#define write_file nai_write_file
#define Cmd Nai_Cmd
#define cmd_append nai_cmd_append
#define cmd_cc nai_cmd_cc
#define cmd_cflags nai_cmd_cflags
#define cmd_run nai_cmd_run
#define arg_shift nai_arg_shift
#define Token_Stream Nai_Token_Stream
#define ts_expect_id nai_ts_expect_id
#define ts_expect_value nai_ts_expect_value
#define ts_expect_type nai_ts_expect_type
#define ts_tok nai_ts_tok
#define ts_get_error nai_ts_get_error
#define ts_balanced nai_ts_balanced
#define ts_match nai_ts_match
#define lexer_to_ts nai_lexer_to_ts
#define add_build_file nai_add_build_file
#define run_build nai_run_build
#define REBUILD NAI_REBUILD

#endif // NAI_FORCE_PREFIX



#endif // NAI_H
