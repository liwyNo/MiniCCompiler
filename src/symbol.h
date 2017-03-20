#ifndef SYMBOL_H
#define SYMBOL_H

#ifdef __cplusplus
extern "C" {
#endif

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

struct __Identifier_t;
union __IdStructure_t;
struct __Typename_t;
typedef const struct __Typename_t *const_Typename_ptr; /* pointer to Typename_t in a symbol stack */

typedef struct __SymbolList_t {
    struct __Identifier_t *id; /* identifer */
    int offset; /* offset from the begging of symbol table */
    struct __SymbolList_t *next; /* pointer to the next identifer */
} SymbolList_t;

typedef struct {
    const_Typename_ptr base_type; /* type of pointer base */
    size_t length; /* only used for array, otherwise set to SIZE_MAX */
} PtrStructure_t;

typedef struct {
    size_t argNum; /* number of arguments */
    /* the following pointer is arrays with length argNum+1 ,
       index 0 is used specially for return type */
    const_Typename_ptr *type;
} FPtrStructure_t;

typedef struct __EnumTable_t {
    char *name; /* enumeration name */
    int value; /* enumeration value */
    struct __EnumTable_t *next; /* pointer to next EnumTable node */
} EnumTable_t;

typedef struct __EnumList_t {
    EnumTable_t *table;
    struct __EnumList_t *next;
} EnumList_t; /* list of EnumTable_t */

typedef union __IdStructure_t {
    SymbolList_t *record;
    PtrStructure_t pointer; // this is used for both pointer and array
    FPtrStructure_t fpointer; /* function pointer */
    const EnumTable_t *enumerate;
} IdStructure_t; /* detailed structure of identifer type */

typedef struct __Typename_t {
    IdType_t type; 
    char *name; /* name of type */
    IdStructure_t *structure; /* detailed type information */
} Typename_t;

typedef struct __Identifier_t {
    char *name; /* name of identifer */
    const_Typename_ptr type; /* type of identifer */
    int size; /* size of identifer */
} Identifier_t;

typedef struct __TypeList_t {
    Typename_t *type; /* typename in this node */
    struct __TypeList_t *next; /* pointer to next typename */
} TypeList_t;

typedef struct __SymbolStack_t {
    SymbolList_t *idList; /* identifers in this chunk */
    TypeList_t *typeList; /* typenames in this chunk */
    EnumList_t *enumList; /* enumerations in this chunk */
    struct __SymbolStack_t *next; /* pointer to next chunk symbol stack node */
} SymbolStack_t;

typedef struct __GotoList_t {
    int offset; /* offset in output */
    struct __GotoList_t *next; /* pointer to next GotoList */
} GotoList_t;

extern SymbolStack_t *symbolStack;

void FreeFPtrStructure(FPtrStructure_t *);
void FreeTypename(Typename_t *);
void FreeIdStructure(IdStructure_t *, int type);
void FreeIdentifier(Identifier_t *);
void FreeSymbolList(SymbolList_t *);
void FreeTypeList(TypeList_t *);
void FreeEnumTable(EnumTable_t *);
void FreeEnumList(EnumList_t *);
void PushSymbolStack();
void PopSymbolStack();
void *LookupSymbol(const char *name, int *symbol_type);
void AddIdentifier(Identifier_t *, SymbolList_t **);
void StackAddIdentifier(Identifier_t *);
void AddTypename(Typename_t *, TypeList_t **);
void StackAddTypename(Typename_t *);
void AddEnumTable(EnumTable_t *, EnumList_t **);
void StackAddEnumTable(EnumTable_t *);

#ifdef __cplusplus
}
#endif

#endif
