#include "yaccTypes.h"
#include "MiniC.tab.h"
#include <string.h>
#include <stdlib.h>

SymbolStack_t *symbolStack = NULL;

void FreePtrStructure(PtrStructure_t *p)
{
    FreeIdStructure(p->base_structure, p->base_type);
}

void FreeFPtrStructure(FPtrStructure_t *p)
{
    for (size_t i = 0; i <= p->argNum; ++i)
        FreeIdStructure(p->structure[i], p->type[i]);
    free(p->structure);
    free(p->type);
}

void FreeIdStructure(IdStructure_t *p, int type)
{
    if (p == NULL)
        return;
    switch (type) {
    case idt_pointer:
        FreePtrStructure(&p->pointer);
        break;
    case idt_fpointer:
        FreeFPtrStructure(&p->fpointer);
        break;
    }
    free(p);
}

void FreeIdentifier(Identifier_t *p)
{
    if (p->name)
        free(p->name);
    FreeIdStructure(p->structure, p->type);
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
        FreeIdentifier(t->type);
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
                *symbol_type = IDENTIFIER;
                return j->id;
            }
        for (TypeList_t *j = i->typeList; j; j = j->next)
            if (strcmp(j->type->name, name) == 0) {
                *symbol_type = TYPE_NAME;
                return j->type;
            }
        for (EnumList_t *j = i->enumList; j; j = j->next)
            for (EnumTable_t *k = j->table; k; k = k->next)
                if (strcmp(k->name, name) == 0) {
                    *symbol_type = ENUM_CONSTANT;
                    return &k->value;
                }
    }
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
