#pragma once

#include "nai.h"

#define NAI_S_FMT "%s"
#define NAI_S_ARG(s) s

#define NAI_S NM(NAI_S, const char *)
#define NAI_SV NM(NAI_SV, Nai_String_View)

#ifndef NM_FORMATS
#define NM_FORMATS
#endif

#define NM_LIST_ NAI_S NAI_SV NM_FORMATS


typedef struct {
    enum {
#define NM(T, type) _NM_TYPE_##T,
        NM_LIST_
#undef NM
    } type;

    union {
#define NM(T, type) type T##_;
        NM_LIST_
#undef NM
    };
} Nai_Fmt_Many;


void nai_sb_print_many_(Nai_String_Builder *sb, Nai_Fmt_Many *many, size_t count);
#define nai_sb_print_many(sb, ...) nai_sb_print_many_(sb, (Nai_Fmt_Many[]){__VA_ARGS__}, sizeof((Nai_Fmt_Many[]){__VA_ARGS__})/sizeof(Nai_Fmt_Many))


void nai_print_many_(Nai_Fmt_Many *many, size_t count);
#define nai_print_many(...) nai_print_many_((Nai_Fmt_Many[]){__VA_ARGS__}, sizeof((Nai_Fmt_Many[]){__VA_ARGS__})/sizeof(Nai_Fmt_Many))


void nai_sb_print_many_(Nai_String_Builder *sb, Nai_Fmt_Many *many, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        switch (many[i].type) {
#define NM(T, type) case _NM_TYPE_##T: { nai_sb_printf(sb, T##_FMT, T##_ARG(many[i].T##_)); break; }
            NM_LIST_
#undef NM
        }
    }
}


void nai_print_many_(Nai_Fmt_Many *many, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        switch (many[i].type) {
#define NM(T, type) case _NM_TYPE_##T: { printf(T##_FMT, T##_ARG(many[i].T##_)); break; }
            NM_LIST_
#undef NM
        }
    }
}

#define nai_println_many(...) nai_print_many(__VA_ARGS__); printf("\n")

#define NM(T, ...) .type = _NM_TYPE_##T, .T##_ =
