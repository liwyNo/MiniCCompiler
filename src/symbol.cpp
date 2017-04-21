#include "symbol.h"
#include "gen.h"
#include "yaccType.h"
#include "yaccUtils.h"
#include "MiniC.tab.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <stack>

SymbolStack_t *symbolStack = NULL;

int type_of_const_exp[IDTYPE_NUM] = {1,1,1,0,1,1};

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
        //FreeTypename(t->type);  // 加上这句会重复delete某段内存
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

void PushSymbolStack(int normal)
{
    SymbolStack_t *p = new SymbolStack_t;
    p->normal = normal;
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
    if (name == NULL) {
        if (symbol_type)
            *symbol_type = 0;
        return NULL;
    }
    for (SymbolStack_t *i = symbolStack; i; i = i->next) {
        for (SymbolList_t *j = i->idList; j; j = j->next)
            if (j->id->name && strcmp(j->id->name, name) == 0) {
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

void *StackHasName(SymbolStack_t *ss, const char *name, int *symbol_type)
{
    if (symbol_type)
        *symbol_type = 0;
    if (name == NULL || strlen(name) == 0)
        return NULL;
    for (SymbolList_t *j = ss->idList; j; j = j->next)
        if (j->id->name && strcmp(j->id->name, name) == 0) {
            if (symbol_type)
                *symbol_type = IDENTIFIER;
            return j->id;
        }
    for (TypeList_t *j = ss->typeList; j; j = j->next)
        if (j->type->name && strcmp(j->type->name, name) == 0) {
            if (symbol_type)
                *symbol_type = TYPE_NAME;
            return j->type;
        }
    for (EnumList_t *j = ss->enumList; j; j = j->next)
        for (EnumTable_t *k = j->table; k; k = k->next)
            if (strcmp(k->name, name) == 0) {
                if (symbol_type)
                    *symbol_type = ENUM_CONSTANT;
                return &k->value;
            }
    return NULL;
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
    if (StackHasName(symbolStack, id->name, NULL))
        yyerror("Identifier name already exists");
    AddIdentifier(id, &symbolStack->idList);
}

void StackAddStaticIdentifier(Identifier_t *id)
{
    SymbolStack_t *p = symbolStack;
    while (p->next)
        p = p->next;
    if (StackHasName(symbolStack, id->name, NULL) || StackHasName(p, id->name, NULL))
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

const_Typename_ptr StackAddTypename(Typename_t *tp)
{
    int stype;
    void *has = StackHasName(symbolStack, tp->name, &stype);
    Typename_t *htp = (Typename_t*)has;
    if (has && (stype != TYPE_NAME || htp->size != -1 || htp->type != tp->type))
        yyerror("Identifier name already exists");
    if (!has) {
        AddTypename(tp, &symbolStack->typeList);
        return tp;
    }
    else {
        *htp = *tp;
        return htp;
    }
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
    AddEnumTable(et, &symbolStack->enumList);
}

void __AddStandardType()
{
    Typename_t *p = new Typename_t;
    p->type = idt_char;
    p->name = strdup("char");
    p->structure = NULL;
    p->size = 1;
    p->isConst = 0;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_short;
    p->name = strdup("short");
    p->size = 2;
    p->isConst = 0;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_int;
    p->name = strdup("int");
    p->isConst = 0;
    p->size = 4;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_long;
    p->isConst = 0;
    p->name = strdup("long");
    p->size = 8;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_uint;
    p->name = strdup("unsigned int");
    p->isConst = 0;
    p->size = 4;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_uchar;
    p->name = strdup("unsigned char");
    p->isConst = 0;
    p->size = 1;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_ushort;
    p->name = strdup("unsigned short");
    p->structure = NULL;
    p->isConst = 0;
    p->size = 2;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_ulong;
    p->name = strdup("unsigned long");
    p->isConst = 0;
    p->size = 8;
    p->structure = NULL;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_float;
    p->name = strdup("float");
    p->isConst = 0;
    p->structure = NULL;
    p->size = 4;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_double;
    p->isConst = 0;
    p->name = strdup("double");
    p->structure = NULL;
    p->size = 8;
    StackAddTypename(p);
    p = new Typename_t;
    p->type = idt_void;
    p->name = strdup("void");
    p->isConst = 0;
    p->structure = NULL;
    p->size = 0;
    StackAddTypename(p);
}

void InitSymbolStack()
{
    varCounter.num_f = 1;
    PushSymbolStack(1);
    __AddStandardType();
}

Identifier_t *StackDeclare(const_Typename_ptr type, int hasSTATIC, int hasTYPEDEF, char *name)
{
    if (hasTYPEDEF && hasSTATIC)
        yyerror("typedef with static");
    if (hasTYPEDEF) {
        Typename_t *ptype = memDup(type);
        ptype->name = name;
        StackAddTypename(ptype);
        return NULL;
    }
    if (type->type == idt_union || type->type == idt_struct)
        if (type->serial_number == -1)
            yyerror("use incomplete struct/union");
    Identifier_t *id = new Identifier_t;
    id->name = name;
    id->type = type;
    SymbolStack_t *ss = symbolStack;
    if (hasSTATIC)
        while (ss->next)
            ss = ss->next;
    if (id->type->type != idt_fpointer) {
        if (ss->normal)
            CreateNativeVar(id, ss);
        else {
            if (StackHasName(ss, id->name, NULL))
                yyerror("Identifier name already exists");
            AddIdentifier(id, &ss->idList);
        }
    }
    else {
        int symbol_type;
        Identifier_t *id2 = (Identifier_t*)LookupSymbol(id->name, &symbol_type);
        if (id2 != NULL && (symbol_type != IDENTIFIER || !sameType(id2->type, id->type)))
            yyerror("identifier already exists");
        else {
            if (id2 == NULL) {
                AddIdentifier(id, &symbolStack->idList);
                CreateFunc(id);
            }
            else
                id = id2;
            id->type->structure->fpointer.implemented = 0;
        }
    }
    id->isConst = 0;
    return id;
}

VarCounter_t varCounter;

int CreateConstant()
{
    return varCounter.num_c++;
}

int CreateTempVar()
{
    // no need to push t\d*
    if (now_func)
        symbol_save.push_back(StackSymbolSave_t('t', varCounter.num_t));
    return varCounter.num_t++;
}

int CreateNativeVar(Identifier_t *id, SymbolStack_t *ss)
{
    char tmp[10];
    sprintf(tmp, "T%d", varCounter.num_T);
    id->TACname = strdup(tmp);
    if (StackHasName(ss, id->name, NULL))
        yyerror("Identifier name already exists");
    AddIdentifier(id, &ss->idList);
    if (now_func)
        symbol_save.push_back(StackSymbolSave_t('T', varCounter.num_T));
    return varCounter.num_T++;
}

int CreateLabel()
{
    return varCounter.num_l++;
}

char* CreateFunc(Identifier_t *id)
{
    id->TACname = strdup(("f_" + std::string(id->name)).c_str());
    return id->TACname;
}

int CreateParam(Identifier_t *id)
{
    char tmp[10];
    sprintf(tmp, "p%d", varCounter.num_p);
    id->TACname = strdup(tmp);
    StackAddIdentifier(id);
    symbol_save.push_back(StackSymbolSave_t('p', varCounter.num_p));
    return varCounter.num_p++;
}

const Identifier_t *now_func = NULL;
std::vector<StackSymbolSave_t> symbol_save;
std::stack<StackSymbolSave_t> symbol_save_real;

void EnterFunc(const Identifier_t *id)
{
    now_func = id;
}

void LeaveFunc()
{
    now_func = NULL;
    varCounter.num_p = 0;
    symbol_save.clear();
}

void PushSymbolSave()
{
    for (auto i : symbol_save) {
        gen_push((i.type + std::to_string(i.num)).c_str());
        symbol_save_real.push(i);
    }
}

void PopSymbolSave()
{
    while (!symbol_save_real.empty()) {
        auto i = symbol_save_real.top();
        symbol_save_real.pop();
        gen_pop((i.type + std::to_string(i.num)).c_str());
    }
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
            *type = (Typename_t *)LookupSymbol("unsigned char", NULL);
        if ((*type)->type == idt_short)
            *type = (Typename_t *)LookupSymbol("unsigned short", NULL);
        if ((*type)->type == idt_int)
            *type = (Typename_t *)LookupSymbol("unsigned int", NULL);
        if ((*type)->type == idt_long)
            *type = (Typename_t *)LookupSymbol("unsigned long", NULL);
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
    if (*sign2 != -1 && *sign2 != sign1 && sign1 != -1) {
        yyerror("type combination error (sign)");
        return;
    }
    if (sign1 != -1)
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
            case idt_int:
            case idt_uint:
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

int type_serial_number = 0;
int NextSerialNumber()
{
    return type_serial_number++;
}

bool sameType(const_Typename_ptr p1, const_Typename_ptr p2)
{
    if (p1 == p2)
        return true;
    if (p1->type != p2->type) {
        if (p1->type == idt_fpointer && p2->type == idt_pointer)
            return sameType(p1, p2->structure->pointer.base_type);
        if (p2->type == idt_fpointer && p1->type == idt_pointer)
            return sameType(p2, p1->structure->pointer.base_type);
        return false;
    } else
        switch (p1->type) {
            case idt_char:
            case idt_uchar:
            case idt_short:
            case idt_ushort:
            case idt_int:
            case idt_uint:
            case idt_long:
            case idt_ulong:
            case idt_float:
            case idt_double:
            case idt_void:
                return true;
            case idt_pointer:
                return sameType(p1->structure->pointer.base_type, p2->structure->pointer.base_type);
            case idt_fpointer:
                if (p1->structure->fpointer.argNum != p2->structure->fpointer.argNum)
                    return false;
                for (size_t i = 0; i <= p1->structure->fpointer.argNum; ++i)
                    if (!sameType(p1->structure->fpointer.type[i], p2->structure->fpointer.type[i]))
                        return false;
                return true;
            case idt_array:
                return p1->structure->pointer.length == p2->structure->pointer.length && sameType(p1->structure->pointer.base_type, p2->structure->pointer.base_type);
            case idt_struct:
            case idt_union:
                /*for (SymbolList_t *i = p1->structure->record, *j = p2->structure->record;; i = i->next, j = j->next) {
                    if (i == NULL && j == NULL)
                        break;
                    if (i == NULL || j == NULL)
                        return false;
                    if (!sameType(i->id->type, j->id->type))
                        return false;
                }
                return true;*/
                return p1->serial_number == p2->serial_number;
            default:
                return false;
        }
}
