#ifndef YACCTYPE_H
#define YACCTYPE_H

#include "symbol.h"

/* synthesize property of XX: XX_s_t
 * inherit property of XX: XX_i_t
 */

typedef struct {
    int hasCONST;
} type_qualifier_s_t;

typedef struct {
    int hasTYPEDEF;
    int hasSTATIC;
} storage_class_specifier_s_t;

typedef struct {
    int sign; // -1 unspecified, 0 unsigned, 1 signed
    const_Typename_ptr type;
} type_specifier_s_t;

typedef struct {
    int hasSTRUCT;
    int hasUNION;
} struct_or_union_s_t;

typedef const_Typename_ptr struct_or_union_specifier_s_t;

typedef struct {
    int hasCONST;
    int sign;
    const_Typename_ptr type;
} specifier_qualifier_list_s_t;

typedef struct {
    int hasTYPEDEF;
    int hasSTATIC;
    int hasCONST;
    int sign;
    const_Typename_ptr type;
} declaration_specifiers_s_t;

typedef declaration_specifiers_s_t init_declarator_list_i_t;

typedef init_declarator_list_i_t init_declarator_i_t;

typedef const_Typename_ptr enum_specifier_s_t;

#endif
