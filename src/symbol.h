#ifndef SYMBOL_H
#define SYMBOL_H

#include <cstddef>
#include <cstdint>
#include <climits>

typedef union {
    int vint;
    long long vlong;
    double vdouble;
    char vchar;
    char *vstr;
} ConstValue_t;

#define IDTYPE_NUM 17
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
    const_Typename_ptr rbase_type; /* type of pointer real base, only used for array */
    const_Typename_ptr base_type; /* type of pointer base */
    int length; /* only used for array, otherwise set to -1, INT_MAX is used for array[]*/
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
    int isConst; /* whether it is constant */
    IdStructure_t *structure; /* detailed type information */
    int size; /* size of typename */
} Typename_t;

typedef struct __Identifier_t {
    char *name; /* name of identifer */
    const_Typename_ptr type; /* type of identifer */
    char *TACname; /* name in three-address-code */
    ConstValue_t value;
    int isConst; /* this is different from type->isConst
                    const int a = 5; // this will set isConst to identifier and typename
                    int f(const int a) {} // this will only set isConst to identifier */
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
    int normal; /* normal means variables in this stack should be generated to output */
} SymbolStack_t;

extern SymbolStack_t *symbolStack;

void FreeFPtrStructure(FPtrStructure_t *);
void FreeTypename(Typename_t *);
void FreeIdStructure(IdStructure_t *, int type);
void FreeIdentifier(Identifier_t *);
void FreeSymbolList(SymbolList_t *);
void FreeTypeList(TypeList_t *);
void FreeEnumTable(EnumTable_t *);
void FreeEnumList(EnumList_t *);
void PushSymbolStack(int normal);
void PopSymbolStack();
void *LookupSymbol(const char *name, int *symbol_type);
void *StackHasName(SymbolStack_t *, const char *, int *);
void AddIdentifier(Identifier_t *, SymbolList_t **);
void StackAddIdentifier(Identifier_t *);
void StackAddStaticIdentifier(Identifier_t *);
void AddTypename(Typename_t *, TypeList_t **);
void StackAddTypename(Typename_t *);
void AddEnumTable(EnumTable_t *, EnumList_t **);
void StackAddEnumTable(EnumTable_t *);
void InitSymbolStack();
struct __declarator_s_t;
Identifier_t *StackDeclare(const_Typename_ptr, int hasSTATIC, int hasTYPEDEF, char *name);

typedef struct {
    int num_c; /* constant */
    int num_t; /* temperary variable */
    int num_T; /* native variable */
    int num_l; /* label */
    int num_f; /* function */
    int num_p; /* parameter */
} VarCounter_t;

extern VarCounter_t varCounter;

int CreateConstant();
int CreateTempVar();
int CreateNativeVar(Identifier_t *, SymbolStack_t *);
int CreateLable();
int CreateFunc(Identifier_t *);
int CreateParam(Identifier_t *);
void CounterLeaveFunc();

int setSign(int, const_Typename_ptr *);
void TypeCombine(int sign1, const_Typename_ptr type1, int *sign2, const_Typename_ptr *type2);
bool sameType(const_Typename_ptr, const_Typename_ptr);

#endif
