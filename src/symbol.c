#include "symbol.h"
#include "MiniC.tab.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

SymbolStack_t *symbolStack = NULL;

void FreeFPtrStructure(FPtrStructure_t *p)
{
    free(p->type);
}

void FreeTypename(Typename_t *p)
{
    if (p->name)
        free(p->name);
    FreeIdStructure(p->structure, p->type);
}

void FreeIdStructure(IdStructure_t *p, int type)
{
    switch (type) {
    case idt_union:
    case idt_struct:
        FreeSymbolList(p->record);
        break;
    case idt_fpointer:
        FreeFPtrStructure(&p->fpointer);
        break;
    }
}

void FreeIdentifier(Identifier_t *p)
{
    free(p->name);
    if (p->TACname)
        free(p->TACname);
}

void FreeSymbolList(SymbolList_t *p)
{
    while (p) {
        SymbolList_t *t = p;
        p = p->next;
        FreeIdentifier(t->id);
        free(t->id);
        free(t);
    }
}

void FreeTypeList(TypeList_t *p)
{
    while (p) {
        TypeList_t *t = p;
        p = p->next;
        FreeTypename(t->type);
        free(t);
    }
}

void FreeEnumTable(EnumTable_t *p)
{
    while (p) {
        EnumTable_t *t = p;
        p = p->next;
        free(t->name);
        free(t);
    }
}
void FreeEnumList(EnumList_t *p)
{
    while (p) {
        EnumList_t *t = p;
        p = p->next;
        FreeEnumTable(t->table);
        free(t);
    }
}

void PushSymbolStack()
{
    SymbolStack_t *p = malloc(sizeof(SymbolStack_t));
    p->idList = NULL;
    p->typeList = NULL;
    p->enumList = NULL;
    p->next = symbolStack;
    symbolStack = p;
}

void PopSymbolStack()
{
    if (symbolStack == NULL)
        return; // error
    SymbolStack_t *p = symbolStack;
    symbolStack = p->next;
    FreeSymbolList(p->idList);
    FreeTypeList(p->typeList);
    FreeEnumList(p->enumList);
    free(p);
}

void *LookupSymbol(const char *name, int *symbol_type)
{
    for (SymbolStack_t *i = symbolStack; i; i = i->next) {
        for (SymbolList_t *j = i->idList; j; j = j->next)
            if (strcmp(j->id->name, name) == 0) {
                if (symbol_type)
                    *symbol_type = IDENTIFIER;
                return j->id;
            }
        for (TypeList_t *j = i->typeList; j; j = j->next)
            if (j->type->name && strcmp(j->type->name, name) == 0) {
                if (symbol_type)
                    *symbol_type = TYPE_NAME;
                return j->type;
            }
        for (EnumList_t *j = i->enumList; j; j = j->next)
            for (EnumTable_t *k = j->table; k; k = k->next)
                if (strcmp(k->name, name) == 0) {
                    if (symbol_type)
                        *symbol_type = ENUM_CONSTANT;
                    return &k->value;
                }
    }
    if (symbol_type)
        *symbol_type = 0;
    return NULL;
}

void AddIdentifier(Identifier_t *id, SymbolList_t **slst)
{
    SymbolList_t *p = malloc(sizeof(SymbolList_t));
    p->id = id;
    p->offset = (*slst ? (*slst)->offset + (*slst)->id->size : 0);
    p->next = *slst;
    *slst = p;
}

void StackAddIdentifier(Identifier_t *id)
{
    AddIdentifier(id, &symbolStack->idList);
}

void StackAddStaticIdentifier(Identifier_t *id)
{
    SymbolStack_t *p = symbolStack;
    while (p->next)
        p = p->next;
    AddIdentifier(id, &p->idList);
}

void AddTypename(Typename_t *tp, TypeList_t **tpl)
{
    TypeList_t *p = malloc(sizeof(TypeList_t));
    p->type = tp;
    p->next = *tpl;
    *tpl = p;
}

void StackAddTypename(Typename_t *tp)
{
    AddTypename(tp, &symbolStack->typeList);
}

void AddEnumTable(EnumTable_t *et, EnumList_t **el)
{
    EnumList_t *p = malloc(sizeof(EnumList_t));
    p->table = et;
    p->next = *el;
    *el = p;
}

void StackAddEnumTable(EnumTable_t *et)
{
    AddEnumTable(et, &symbolStack->enumList);
}

void __AddStandardType()
{
    Typename_t *p;
    p = malloc(sizeof(Typename_t));
    p->type = idt_char;
    p->name = strdup("char");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_short;
    p->name = strdup("short");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_int;
    p->name = strdup("int");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_long;
    p->name = strdup("long");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_uchar;
    p->name = strdup("uchar");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_ushort;
    p->name = strdup("ushort");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_uint;
    p->name = strdup("uint");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_ulong;
    p->name = strdup("ulong");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_float;
    p->name = strdup("float");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_double;
    p->name = strdup("double");
    p->structure = NULL;
    StackAddTypename(p);
    p = malloc(sizeof(Typename_t));
    p->type = idt_void;
    p->name = strdup("void");
    p->structure = NULL;
    StackAddTypename(p);
}

void InitSymbolStack()
{
    PushSymbolStack();
    __AddStandardType();
}

VarCounter_t varCounter;

int CreateConstant()
{
    return varCounter.num_c++;
}

int CreateTempVar()
{
    return varCounter.num_t++;
}

int CreateNativeVar(Identifier_t *id)
{
    char tmp[10];
    sprintf(tmp, "T%d", varCounter.num_T);
    id->TACname = strdup(tmp);
    StackAddIdentifier(id);
    return varCounter.num_T++;
}

int CreateLable()
{
    return varCounter.num_l++;
}

int CreateFunc(Identifier_t *id)
{
    char tmp[10];
    sprintf(tmp, "f%d", varCounter.num_f);
    id->TACname = strdup(tmp);
    StackAddIdentifier(id);
    return varCounter.num_f++;
}

int CreateParam(Identifier_t *id)
{
    char tmp[10];
    sprintf(tmp, "p%d", varCounter.num_p);
    id->TACname = strdup(tmp);
    StackAddIdentifier(id);
    return varCounter.num_p++;
}

void CounterLeaveFunc()
{
    varCounter.num_p = 0;
}
