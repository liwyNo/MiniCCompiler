#ifndef YACCTYPES_H
#define YACCTYPES_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    idt_char, idt_short, idt_int, idt_long, /* signed integer */
    idt_uchar, idt_ushort, idt_uint, idt_ulong, /* unsigned integer */
    idt_float, idt_double, /* floating-point number */
    idt_pointer, idt_fpointer, /* pointer & function pointer */
    idt_void, /* void */
    idt_array, /* array */
    idt_struct, idt_union, /* structured record */
    idt_enum /* enumeration */
} IdType_t;

struct __Identifer_t;
union __IdStructure_t;

typedef struct __SymbolList_t {
    __Identifer_t *id; /* identifer */
    int offset; /* offset from the begging of symbol table */
    __SymbolList_t *next; /* pointer to the next identifer */
} SymbolList_t;

typedef struct {
    IdType_t base_type;
    __IdStructure_t *base_structure;
    size_t length; /* only used for array, otherwise set to SIZE_MAX */
} PtrStructure_t;

typedef struct {
    size_t argNum; /* number of arguments */
    /* the following two pointer are arrays with length argNum+1 ,
       index 0 is used specially for return type */
    IdType_t *type;
    __IdStructure_t *structure;
} FPtrStructure_t;

typedef struct {
    int num; /* number of names in enum */
    char **name; /* each name in enum */
    int *value; /* exact value of each name */
} EnumTable_t;

typedef union __IdStructure_t {
    SymbolList_t record;
    PtrStructure_t pointer; // this is used for both pointer and array
    FPtrStructure_t fpointer; /* function pointer */
    EnumTable_t enumerate;
} IdStructure_t; /* detailed structure of identifer type */

typedef struct __Identifer_t {
    char *name; /* name of identifer */
    IdType_t type; /* type of identifer */
    IdStructure_t *structure; /* detailed type of identifer */
    int size; /* size of identifer */
} Identifer_t;

typedef Identifer_t Typename_t; // Typename_t is used for typedef statements

typedef struct __GotoList_t {
    int lineNum; /* line number in output */
    struct __GotoList_t *next; /* pointer to next GotoList */
} GotoList_t;

#endif
