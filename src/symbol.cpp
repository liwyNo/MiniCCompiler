#include "symbol.h"
#include "yaccType.h"
#include "yaccUtils.h"
#include "MiniC.tab.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>

SymbolStack_t *symbolStack = NULL;

void FreeFPtrStructure(FPtrStructure_t *p)
{
    delete[] p->type;
}

void FreeTypename(Typename_t *p)
{
    if (p->name)
        delete p->name;
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
    delete p->name;
    if (p->TACname)
        delete p->TACname;
}

void FreeSymbolList(SymbolList_t *p)
{
    while (p) {
        SymbolList_t *t = p;
        p = p->next;
        FreeIdentifier(t->id);
        delete t->id;
        delete t;
    }
}

void FreeTypeList(TypeList_t *p)
{
    while (p) {
        TypeList_t *t = p;
        p = p->next;
        FreeTypename(t->type);
        delete t;
    }
}

void FreeEnumTable(EnumTable_t *p)
{
    while (p) {
        EnumTable_t *t = p;
        p = p->next;
        delete t->name;
        delete t;
    }
}
void FreeEnumList(EnumList_t *p)
{
    while (p) {
        EnumList_t *t = p;
        p = p->next;
        FreeEnumTable(t->table);
        delete t;
    }
}

void PushSymbolStack()
{
    SymbolStack_t *p = new SymbolStack_t;
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
    delete p;
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

int StackHasName(SymbolStack_t *ss, const char *name)
{
    if (name == NULL || strlen(name) == 0)
        return 0;
    for (SymbolList_t *j = ss->idList; j; j = j->next)
        if (strcmp(j->id->name, name) == 0)
            return 1;
    for (TypeList_t *j = ss->typeList; j; j = j->next)
        if (j->type->name && strcmp(j->type->name, name) == 0)
            return 1;
    for (EnumList_t *j = ss->enumList; j; j = j->next)
        for (EnumTable_t *k = j->table; k; k = k->next)
            if (strcmp(k->name, name) == 0)
                return 1;
    return 0;
}

void AddIdentifier(Identifier_t *id, SymbolList_t **slst)
{
    SymbolList_t *p = new SymbolList_t;
    p->id = id;
    p->offset = (*slst ? (*slst)->offset + (*slst)->id->type->size : 0);
    p->next = *slst;
    *slst = p;
}

void StackAddIdentifier(Identifier_t *id)
{
    if (StackHasName(symbolStack, id->name))
        yyerror("Identifier name already exists");
    AddIdentifier(id, &symbolStack->idList);
}

void StackAddStaticIdentifier(Identifier_t *id)
{
    SymbolStack_t *p = symbolStack;
    while (p->next)
        p = p->next;
    if (StackHasName(symbolStack, id->name) || StackHasName(p, id->name))
        yyerror("Identifier name already exists");
    AddIdentifier(id, &p->idList);
}

void AddTypename(Typename_t *tp, TypeList_t **tpl)
{
    TypeList_t *p = new TypeList_t;
    p->type = tp;
    p->next = *tpl;
    *tpl = p;
}

void StackAddTypename(Typename_t *tp)
{
    if (StackHasName(symbolStack, tp->name))
        yyerror("Identifier name already exists");
    AddTypename(tp, &symbolStack->typeList);
}

void AddEnumTable(EnumTable_t *et, EnumList_t **el)
{
    EnumList_t *p = new EnumList_t;
    p->table = et;
    p->next = *el;
    *el = p;
}

void StackAddEnumTable(EnumTable_t *et)
{
    if (StackHasName(symbolStack, et->name))
        yyerror("Identifier name already exists");
    AddEnumTable(et, &symbolStack->enumList);
}

void __AddStandardType()
{
    Typename_t *p = new Typename_t;
    p->type = idt_char;
    p->name = strdup("char");
    p->structure = NULL;
    p->size = 1;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_short;
    p->name = strdup("short");
    p->size = 2;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_int;
    p->name = strdup("int");
    p->size = 4;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_long;
    p->name = strdup("long");
    p->size = 8;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_uchar;
    p->name = strdup("uchar");
    p->size = 1;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_ushort;
    p->name = strdup("ushort");
    p->structure = NULL;
    p->size = 2;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_uint;
    p->name = strdup("uint");
    p->size = 4;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_ulong;
    p->name = strdup("ulong");
    p->size = 8;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_float;
    p->name = strdup("float");
    p->structure = NULL;
    p->size = 4;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_double;
    p->name = strdup("double");
    p->structure = NULL;
    p->size = 8;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_void;
    p->name = strdup("void");
    p->structure = NULL;
    p->size = 0;
    StackAddTypename(p);
}

void InitSymbolStack()
{
    PushSymbolStack();
    __AddStandardType();
}

void StackDeclare(const_Typename_ptr type, int hasSTATIC, int hasTYPEDEF, declarator_s_t decl)
{
    if (hasTYPEDEF && hasSTATIC)
        yyerror("typedef with const or static");
    Typename_t *ptype = makeType(type, decl);
    char *idname;
    { /* get idname */
        direct_declarator_s_t *i = decl.dd;
        while (i->type != 1)
            switch (i->type) {
            case 2:
                i = i->data.d2.dd;
                break;
            case 3:
                i = i->data.d3;
                break;
            case 4:
                i = i->data.d4.dd;
                break;
            case 5:
                i = i->data.d5.dd;
                break;
            case 6:
                i = i->data.d6;
                break;
            }
        idname = i->data.d1;
    }
    if (hasTYPEDEF) {
        ptype->name = idname;
        StackAddTypename(ptype);
        return;
    }
    Identifier_t *id = new Identifier_t;
    id->name = idname;
    id->type = ptype;
    SymbolStack_t *ss = symbolStack;
    if (hasSTATIC)
        while (ss->next)
            ss = ss->next;
    if (id->type->type == idt_fpointer)
        CreateFunc(id);
    else
        CreateNativeVar(id, ss);
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

int CreateNativeVar(Identifier_t *id, SymbolStack_t *ss)
{
    char tmp[10];
    sprintf(tmp, "T%d", varCounter.num_T);
    id->TACname = strdup(tmp);
    if (StackHasName(ss, id->name))
        yyerror("Identifier name already exists");
    AddIdentifier(id, &ss->idList);
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

int __isIntegerType(IdType_t t)
{
    return t >= idt_char && t <= idt_ulong;
}
int setSign(int sign, const_Typename_ptr *type)
{
    if (!__isIntegerType((*type)->type))
        return 0;
    if (sign == 0) {
        if ((*type)->type == idt_char)
            *type = (Typename_t *)LookupSymbol("uchar", NULL);
        if ((*type)->type == idt_short)
            *type = (Typename_t *)LookupSymbol("ushort", NULL);
        if ((*type)->type == idt_int)
            *type = (Typename_t *)LookupSymbol("uint", NULL);
        if ((*type)->type == idt_long)
            *type = (Typename_t *)LookupSymbol("ulong", NULL);
    }
    else if (sign == 1) {
        if ((*type)->type == idt_uchar)
            *type = (Typename_t *)LookupSymbol("char", NULL);
        if ((*type)->type == idt_ushort)
            *type = (Typename_t *)LookupSymbol("short", NULL);
        if ((*type)->type == idt_uint)
            *type = (Typename_t *)LookupSymbol("int", NULL);
        if ((*type)->type == idt_ulong)
            *type = (Typename_t *)LookupSymbol("long", NULL);
    }
    return 1;
}

void TypeCombine(int sign1, const_Typename_ptr type1, int *sign2, const_Typename_ptr *type2)
{
    if (*sign2 != -1 && *sign2 != sign1) {
        yyerror("type combination error (sign)");
        return;
    }
    *sign2 = sign1;
    if (*type2 == NULL)
        *type2 = type1;
    else if (type1 != NULL) {
        if ((*type2)->type != idt_int && (*type2)->type != idt_uint) {
            if ((type1->type != idt_int && type1->type != idt_uint) || !__isIntegerType((*type2)->type)) {
                yyerror("type combination error (int)");
                return;
            }
        }
        else
            switch (type1->type) {
            case idt_char:
            case idt_uchar:
                *type2 = (Typename_t *)LookupSymbol("char", NULL);
                break;
            case idt_short:
            case idt_ushort:
                *type2 = (Typename_t *)LookupSymbol("short", NULL);
                break;
            case idt_long:
            case idt_ulong:
                *type2 = (Typename_t *)LookupSymbol("long", NULL);
                break;
            default:
                yyerror("type combination error");
                return;
            }
    }
    if (*sign2 != -1 && !setSign(*sign2, type2)) {
        yyerror("type combination error (sign)");
        return;
    }
}
