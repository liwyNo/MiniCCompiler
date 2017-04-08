#ifndef YACCTYPE_H
#define YACCTYPE_H

#include "symbol.h"
/* synthesize property of XX: XX_s_t
 * inherit property of XX: XX_i_t
 */

typedef struct {
    int isConst;
    const_Typename_ptr type;
    bool lr_value; //left : 0,right :1
    ConstValue_t value; /* only use for constant */
    char *addr; /* 直接有变量名的东西，就用addr存 */
    char *laddr; /*只能用指针访问的东西，就存它的指针，以上两者一定是不共存的，即一定有一个为NULL

    注意：array和struct/union是特例！！！他们没有实体，所以他们的本身的值就是一个地址，所以他们全都统一用addr,没有laddr
    
    */
    char *get_addr() const;//得到该表达式的真正的addr
} expression_s_t;

typedef struct Argument_Expression_List_s_t{
    int length;
    expression_s_t now_exp;
    Argument_Expression_List_s_t *next;
} argument_expression_list_s_t;

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

typedef const_Typename_ptr enum_specifier_s_t;

typedef EnumTable_t *enumerator_list_s_t;

typedef struct {
    char *name;
    int value;
} enumerator_s_t;

typedef struct __pointer_list_t {
    int hasConst;
    struct __pointer_list_t *next;
} pointer_list_t;
typedef pointer_list_t *pointer_s_t;
void freePL(pointer_list_t *);

struct __direct_declarator_s_t;

typedef struct __declarator_s_t {
    pointer_s_t ptr;
    struct __direct_declarator_s_t *dd;
} declarator_s_t;

typedef SymbolStack_t *parameter_list_s_t;

typedef struct __direct_declarator_s_t {
    int type; // 1 ~ 6
    union {
        char *d1;
        declarator_s_t d2;
        struct __direct_declarator_s_t *d3;
        struct {
            struct __direct_declarator_s_t *dd;
            int ce;
        } d4;
        struct {
            struct __direct_declarator_s_t *dd;
            parameter_list_s_t pl;
        } d5;
        struct __direct_declarator_s_t *d6;
    } data;
} direct_declarator_s_t;
char *getDeclaratorName(const declarator_s_t *);
void freeDD(direct_declarator_s_t *);

typedef struct __struct_declarator_list_s_t {
    declarator_s_t decl;
    struct __struct_declarator_list_s_t *next;
} struct_declarator_list_s_t;
void freeSDL(struct_declarator_list_s_t *);

struct __direct_abstract_declarator_s_t;

typedef struct {
    pointer_s_t ptr;
    struct __direct_abstract_declarator_s_t *dad;
} abstract_declarator_s_t;

typedef struct __direct_abstract_declarator_s_t {
    int type; // 1 ~ 9
    union {
        abstract_declarator_s_t d1;
        int d3;
        struct __direct_abstract_declarator_s_t *d4;
        struct {
            struct __direct_abstract_declarator_s_t *dad;
            int ce;
        } d5;
        parameter_list_s_t d7;
        struct __direct_abstract_declarator_s_t *d8;
        struct {
            struct __direct_abstract_declarator_s_t *dad;
            parameter_list_s_t pl;
        } d9;
    } data;
} direct_abstract_declarator_s_t;

typedef const_Typename_ptr type_name_s_t;

Typename_t *makeType(const_Typename_ptr type, declarator_s_t decl);
Typename_t *makeType(const_Typename_ptr type, abstract_declarator_s_t ad);

struct __initializer_list_s_t;

typedef struct __initializer_s_t {
    struct __initializer_list_s_t *lst;
    expression_s_t data;
} initializer_s_t;
void freeInit(initializer_s_t *);

typedef struct __initializer_list_s_t {
    initializer_s_t data;
    struct __initializer_list_s_t *next;
} initializer_list_s_t;
void freeIL(initializer_list_s_t *);

typedef struct {
    declarator_s_t decl;
    initializer_s_t *init;
} init_declarator_s_t;
void freeID(init_declarator_s_t *);

typedef struct __init_declarator_list_s_t {
    init_declarator_s_t idecl;
    struct __init_declarator_list_s_t *next;
} init_declarator_list_s_t;
void freeIDL(init_declarator_list_s_t *);

typedef struct {
    SymbolList_t *sblst;
    bool has_begin, has_end;
    int begin_num, end_num;
} statement_i_t;

typedef struct __CaseList_t {
    bool isDefault;
    const char *value;
    IdType_t type;
    int label;
    struct __CaseList_t *next;
} CaseList_t;

typedef struct {
    CaseList_t *caseList;
} statement_s_t;

typedef struct {
    bool have;
    expression_s_t expr;
} expression_statement_s_t;

typedef struct {
    int lb_end;
    int lb_state;
    int lb_iter;
} for_jumper2_s_t;

#endif
