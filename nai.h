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

#define nai_log_info(fmt, ...) printf("\x1b[36m[INFO]\x1b[0m "fmt"\n", ##__VA_ARGS__)

#define nai_log_help(fmt, ...) printf("\x1b[32m[HELP]\x1b[0m\x1b[2m "fmt"\x1b[0m\n", ##__VA_ARGS__)

#define nai_log_warning(fmt, ...) printf("\x1b[33m[WARNING]\x1b[0m "fmt"\n", ##__VA_ARGS__)

#define nai_log_error(fmt, ...) printf("\x1b[31m[ERROR]\x1b[0m "fmt"\n", ##__VA_ARGS__)


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

#define NAI_SV_FMT "%.*s"
#define NAI_SV_ARG(sv) (int) (sv).count, (sv).data

Nai_String_View nai_sb_to_sv(Nai_String_Builder sb);
Nai_String_Builder nai_sv_to_sb(Nai_String_View sv);
bool nai_sv_equals(Nai_String_View sv, const char *cstr);
Nai_String_View nai_sv_chop_space(Nai_String_View *sv);
Nai_String_View nai_sv_chop(Nai_String_View *sv, char delim);
Nai_String_View nai_sv_split(Nai_String_View *sv, const char *delim);
Nai_String_View nai_sv_trim(Nai_String_View sv);
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


#define nai_array_foreach(type, x, array) for (type *x = (array)->items; x != (array)->items + (array)->count; ++x)


void nai_recompile_metaprogram(int argc, char **argv, const char *file_name);
void nai_rebuild_metaprogram_if_needed(int argc, char **argv, const char *file_name);


typedef Nai_Array(const char *) Nai_Cmd;

void nai_cmd_append_(Nai_Cmd *cmd, ...);
#define nai_cmd_append(cmd, ...) nai_cmd_append_(cmd, __VA_ARGS__, 0)

Nai_String_Builder nai_cmd_render(Nai_Cmd cmd);

typedef struct {
    Nai_Cmd cmd;
    bool debug;
} Nai_Cmd_Run_Params_;

int nai_cmd_run_(Nai_Cmd_Run_Params_ ps);
#define nai_cmd_run(...) nai_cmd_run_((Nai_Cmd_Run_Params_){__VA_ARGS__})


#define NAI_REBUILD(argc, argv) nai_rebuild_metaprogram_if_needed(argc, argv, __FILE__)


Nai_String_Builder nai_read_line(void);
const char *sprint_number(double f);

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

Nai_String_View nai_sv_trim(Nai_String_View sv)
{
    while (isspace(*sv.data)) {
        ++sv.data;
        --sv.count;
    }

    while (isspace(sv.data[sv.count - 1])) {
        --sv.count;
    }

    return sv;
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

Nai_String_View nai_sv_split(Nai_String_View *sv, const char *delim)
{
    const char *data = sv->data;
    size_t len = strlen(delim);
    
    while (sv->count) {
        if (!strncmp(sv->data, delim, len)) {
            Nai_String_View chopped = {
                .data = data,
                .count = sv->data - data,
            };

            sv->data += len;
            sv->count -= len;

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

bool nai_read_file_sv(Nai_String_View path, Nai_String_Builder *out)
{
    Nai_String_Builder sb = {0};
    nai_sb_printf(&sb, NAI_SV_FMT, NAI_SV_ARG(path));
    nai_sb_print_null(&sb);

    return nai_read_file(sb.data, out);
}


bool nai_write_file(const char *file_path, Nai_String_View content)
{
    FILE *file = fopen(file_path, "w+");
    if (!file) return false;

    fwrite(content.data, 1, content.count, file);
    fclose(file);

    return true;
}


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

Nai_String_Builder nai_cmd_render(Nai_Cmd cmd)
{
    Nai_String_Builder sb = {0};
    
    nai_array_foreach(const char *, arg, &cmd) {
        nai_sb_printf(&sb, "%s ", *arg);
    }

    return sb;
}

int nai_cmd_run_(Nai_Cmd_Run_Params_ ps)
{
    Nai_String_Builder sb = nai_cmd_render(ps.cmd);

    if (ps.debug) {
        nai_log_info("\x1b[2m+ "NAI_SV_FMT"\x1b[0m", NAI_SV_ARG(sb));
    }

    nai_sb_print_null(&sb);
    return system(sb.data);
}

Nai_String_Builder nai_read_line(void)
{
    fflush(stdout);

    char buffer[8 * 1024];
    Nai_String_Builder sb = {0};

    fgets(buffer, sizeof(buffer), stdin);

    size_t len = strlen(buffer) - 1;
    nai_sb_reserve(&sb, len);
    memcpy(sb.data, buffer, len);
    sb.count = len;

    return sb;
}


#include <math.h>

const char *nai_sprint_number(double f)
{
    const char *fmt;
    double x = fabs(f);

    if (fabs(round(x) - x) < 1e-2) fmt = "%.0lf";
    else if (fabs(round(x*1e1)*1e-1 - x) < 1e-3) fmt = "%.1lf";
    else if (fabs(round(x*1e2)*1e-2 - x) < 1e-4) fmt = "%.2lf";
    else if (fabs(round(x*1e3)*1e-3 - x) < 1e-5) fmt = "%.3lf";
    else if (fabs(round(x*1e4)*1e-4 - x) < 1e-6) fmt = "%.4lf";
    else if (fabs(round(x*1e5)*1e-5 - x) < 1e-8) fmt = "%.5lf";
    else if (fabs(round(x*1e6)*1e-6 - x) < 1e-9) fmt = "%.6lf";
    else if (fabs(round(x*1e7)*1e-7 - x) < 1e-10) fmt = "%.7lf";
    else if (fabs(round(x*1e8)*1e-8 - x) < 1e-11) fmt = "%.8lf";
    else if (fabs(round(x*1e9)*1e-9 - x) < 1e-12) fmt = "%.9lf";
    else fmt = "%.10lf";

    return nai_temp_sprintf(fmt, f);
}

#endif // NAI_HEADER_ONLY


#ifndef NAI_FORCE_PREFIX

#define TODO NAI_TODO
#define log_info nai_log_info
#define log_help nai_log_help
#define log_warning nai_log_warning
#define log_error nai_log_error
#define Arena Nai_Arena
#define global_arena nai_global_arena
#define init_global_arena nai_init_global_arena
#define global_arena_alloc nai_global_arena_alloc
#define String_Builder Nai_String_Builder
#define sb_reserve nai_sb_reserve
#define temp_sprintf nai_temp_sprintf
#define sb_printf nai_sb_printf
#define sb_print_null nai_sb_print_null
#define sb_print_many nai_sb_print_many
#define String_View Nai_String_View
#define SV_FMT NAI_SV_FMT
#define SV_ARG NAI_SV_ARG
#define sb_to_sv nai_sb_to_sv
#define sv_to_sb nai_sv_to_sb
#define sv_equals nai_sv_equals
#define sv_trim nai_sv_trim
#define sv_chop nai_sv_chop
#define sv_split nai_sv_split
#define sv_chop_space nai_sv_chop_space
#define sv_chop_right nai_sv_chop_right
#define sv_from_cstr nai_sv_from_cstr
#define Array Nai_Array
#define array_append nai_array_append
#define array_foreach nai_array_foreach
#define read_file nai_read_file
#define read_file_sv nai_read_file_sv
#define write_file nai_write_file
#define arg_shift nai_arg_shift
#define Cmd Nai_Cmd
#define cmd_append nai_cmd_append
#define cmd_render nai_cmd_render
#define cmd_run nai_cmd_run
#define REBUILD NAI_REBUILD
#define read_line nai_read_line
#define sprint_number nai_sprint_number

#endif // NAI_FORCE_PREFIX



#endif // NAI_H
