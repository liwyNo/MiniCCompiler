#include "yaccType.h"
#include "MiniC.tab.hpp"
#include <cstdlib>
#include "yaccUtils.h"
#include "gen.h"

char* expression_s_t::get_addr() const
{
    if(addr != NULL)
        return addr;
    else
    {
        char *tvar = get_TAC_name('t',CreateTempVar());
        gen_var(map_name[type -> type], tvar);
        gen_op1(tvar, laddr, "*");
        return tvar;
    }
}

void freePL(pointer_list_t *p)
{
    while (p) {
        pointer_list_t *t = p;
        p = p->next;
        delete t;
    }
}

void freeDD(direct_declarator_s_t *p)
{
    switch (p->type) {
    case 3:
        delete p->data.d3;
        break;
    case 4:
        delete p->data.d4.dd;
        break;
    case 5:
        delete p->data.d5.dd;
        break;
    case 6:
        delete p->data.d6;
        break;
    }
}

void freeSDL(struct_declarator_list_s_t *p)
{
    while (p) {
        struct_declarator_list_s_t *t = p;
        p = p->next;
        freeDD(t->decl.dd);
        freePL(t->decl.ptr);
        delete t;
    }
}

Typename_t *makeType(const_Typename_ptr type, declarator_s_t decl)
{
    Typename_t *tmp = new Typename_t;
    declarator_s_t decl2;
    if (decl.ptr) {
        tmp->isConst = decl.ptr->hasConst;
        tmp->type = idt_pointer;
        tmp->structure = new IdStructure_t;
        tmp->structure->pointer.base_type = type;
        tmp->structure->pointer.length = -1;
        tmp->size = 4;
        tmp->name = NULL;
        decl2.ptr = decl.ptr->next;
        decl2.dd = decl.dd;
        return makeType(tmp, decl2);
    }
    else {
        tmp->isConst = 0;
        int argNum;
        switch (decl.dd->type) {
        case 1:
            *tmp = *type;
            return tmp;
        case 2:
            delete tmp;
            return makeType(type, decl.dd->data.d2);
        case 3:
            tmp->type = idt_pointer;
            tmp->structure = new IdStructure_t;
            tmp->structure->pointer.base_type = type;
            tmp->structure->pointer.length = INT_MAX;
            if (type->type == idt_pointer && type->structure->pointer.length == INT_MAX)
                yyerror("[] pointer error");
            tmp->size = 4;
            decl2.ptr = NULL;
            decl2.dd = decl.dd->data.d3;
            return makeType(tmp, decl2);
        case 4:
            tmp->type = idt_array;
            tmp->structure = new IdStructure_t;
            tmp->structure->pointer.base_type = type;
            tmp->structure->pointer.length = decl.dd->data.d4.ce;
            tmp->size = type->size * decl.dd->data.d4.ce;
            if (type->type == idt_pointer && type->structure->pointer.length == INT_MAX)
                yyerror("[] pointer error");
            if (type->type == idt_array)
                tmp->structure->pointer.rbase_type = type->structure->pointer.rbase_type;
            else
                tmp->structure->pointer.rbase_type = type;
            decl2.ptr = NULL;
            decl2.dd = decl.dd->data.d4.dd;
            return makeType(tmp, decl2);
        case 5:
            if (type->type == idt_fpointer)
                yyerror("function can't return function");
            tmp->type = idt_fpointer;
            tmp->structure = new IdStructure_t;
            argNum = 0;
            for (SymbolList_t *i = decl.dd->data.d5.pl->idList; i; i = i->next)
                ++argNum;
            tmp->structure->fpointer.argNum = argNum;
            tmp->structure->fpointer.type = new const_Typename_ptr[argNum + 1] {type};
            argNum = 1;
            for (SymbolList_t *i = decl.dd->data.d5.pl->idList; i; i = i->next)
                tmp->structure->fpointer.type[argNum++] = i->id->type;
            tmp->size = 4;
            decl2.ptr = NULL;
            decl2.dd = decl.dd->data.d5.dd;
            return makeType(tmp, decl2);
        case 6:
            if (type->type == idt_fpointer)
                yyerror("function can't return function");
            tmp->type = idt_fpointer;
            tmp->structure = new IdStructure_t;
            tmp->structure->fpointer.argNum = 0;
            tmp->structure->fpointer.type = new const_Typename_ptr[1] {type};
            tmp->size = 4;
            decl2.ptr = NULL;
            decl2.dd = decl.dd->data.d6;
            return makeType(tmp, decl2);
        }
    }
    return NULL;
}

Typename_t *makeType(const_Typename_ptr type, abstract_declarator_s_t ad)
{
    Typename_t *tmp = new Typename_t;
    abstract_declarator_s_t ad2;
    if (ad.ptr) {
        tmp->isConst = ad.ptr->hasConst;
        tmp->type = idt_pointer;
        tmp->structure = new IdStructure_t;
        tmp->structure->pointer.base_type = type;
        tmp->structure->pointer.length = -1;
        tmp->size = 4;
        tmp->name = NULL;
        ad2.ptr = ad.ptr->next;
        ad2.dad = ad.dad;
        return makeType(tmp, ad2);
    }
    else {
        if (ad.dad == NULL) {
            *tmp = *type;
            return tmp;
        }
        tmp->isConst = 0;
        int argNum;
        switch (ad.dad->type) {
        case 1:
            delete tmp;
            return makeType(type, ad.dad->data.d1);
        case 2:
            tmp->type = idt_pointer;
            tmp->structure = new IdStructure_t;
            tmp->structure->pointer.base_type = type;
            tmp->structure->pointer.length = INT_MAX;
            if (type->type == idt_pointer && type->structure->pointer.length == INT_MAX)
                yyerror("[] pointer error");
            tmp->size = 4;
            return tmp;
        case 3:
            tmp->type = idt_array;
            tmp->structure = new IdStructure_t;
            tmp->structure->pointer.base_type = type;
            tmp->structure->pointer.length = ad.dad->data.d3;
            tmp->size = type->size * ad.dad->data.d3;
            if (type->type == idt_pointer && type->structure->pointer.length == INT_MAX)
                yyerror("[] pointer error");
            if (type->type == idt_array)
                tmp->structure->pointer.rbase_type = type->structure->pointer.rbase_type;
            else
                tmp->structure->pointer.rbase_type = type;
            return tmp;
        case 4:
            tmp->type = idt_pointer;
            tmp->structure = new IdStructure_t;
            tmp->structure->pointer.base_type = type;
            tmp->structure->pointer.length = INT_MAX;
            if (type->type == idt_pointer && type->structure->pointer.length == INT_MAX)
                yyerror("[] pointer error");
            tmp->size = 4;
            ad2.ptr = NULL;
            ad2.dad = ad.dad->data.d4;
            return makeType(tmp, ad2);
        case 5:
            tmp->type = idt_array;
            tmp->structure = new IdStructure_t;
            tmp->structure->pointer.base_type = type;
            tmp->structure->pointer.length = ad.dad->data.d5.ce;
            tmp->size = type->size * ad.dad->data.d5.ce;
            if (type->type == idt_pointer && type->structure->pointer.length == INT_MAX)
                yyerror("[] pointer error");
            if (type->type == idt_array)
                tmp->structure->pointer.rbase_type = type->structure->pointer.rbase_type;
            else
                tmp->structure->pointer.rbase_type = type;
            ad2.ptr = NULL;
            ad2.dad = ad.dad->data.d5.dad;
            return makeType(tmp, ad2);
        case 6:
            if (type->type == idt_fpointer)
                yyerror("function can't return function");
            tmp->type = idt_fpointer;
            tmp->structure = new IdStructure_t;
            tmp->structure->fpointer.argNum = 0;
            tmp->structure->fpointer.type = new const_Typename_ptr[1] {type};
            tmp->size = 4;
            return tmp;
        case 7:
            if (type->type == idt_fpointer)
                yyerror("function can't return function");
            tmp->type = idt_fpointer;
            tmp->structure = new IdStructure_t;
            argNum = 0;
            for (SymbolList_t *i = ad.dad->data.d7->idList; i; i = i->next)
                ++argNum;
            tmp->structure->fpointer.argNum = argNum;
            tmp->structure->fpointer.type = new const_Typename_ptr[argNum + 1] {type};
            argNum = 1;
            for (SymbolList_t *i = ad.dad->data.d7->idList; i; i = i->next)
                tmp->structure->fpointer.type[argNum++] = i->id->type;
            tmp->size = 4;
            return tmp;
        case 8:
            if (type->type == idt_fpointer)
                yyerror("function can't return function");
            tmp->type = idt_fpointer;
            tmp->structure = new IdStructure_t;
            tmp->structure->fpointer.argNum = 0;
            tmp->structure->fpointer.type = new const_Typename_ptr[1] {type};
            tmp->size = 4;
            ad2.ptr = NULL;
            ad2.dad = ad.dad->data.d8;
            return makeType(tmp, ad2);
        case 9:
            if (type->type == idt_fpointer)
                yyerror("function can't return function");
            tmp->type = idt_fpointer;
            tmp->structure = new IdStructure_t;
            argNum = 0;
            for (SymbolList_t *i = ad.dad->data.d9.pl->idList; i; i = i->next)
                ++argNum;
            tmp->structure->fpointer.argNum = argNum;
            tmp->structure->fpointer.type = new const_Typename_ptr[argNum + 1] {type};
            argNum = 1;
            for (SymbolList_t *i = ad.dad->data.d9.pl->idList; i; i = i->next)
                tmp->structure->fpointer.type[argNum++] = i->id->type;
            tmp->size = 4;
            ad2.ptr = NULL;
            ad2.dad = ad.dad->data.d9.dad;
            return makeType(tmp, ad2);
        }
    }
    return NULL;
}

char *getDeclaratorName(const declarator_s_t *d)
{
    direct_declarator_s_t *dd = d->dd;
    while (dd->type != 1)
        switch (dd->type) {
        case 2:
            dd = dd->data.d2.dd;
            break;
        case 3:
            dd = dd->data.d3;
            break;
        case 4:
            dd = dd->data.d4.dd;
            break;
        case 5:
            dd = dd->data.d5.dd;
            break;
        case 6:
            dd = dd->data.d6;
            break;
        }
    return dd->data.d1;
}

void freeInit(initializer_s_t *p)
{
    if (p->lst)
        freeIL(p->lst);
}

void freeIL(initializer_list_s_t *p)
{
    while (p)
    {
        initializer_list_s_t *t = p;
        p = p->next;
        freeInit(&t->data);
        delete t;
    }
}

void freeID(init_declarator_s_t *p)
{
    if (p->init)
        freeInit(p->init);
    freeDD(p->decl.dd);
}
void freeIDL(init_declarator_list_s_t *p)
{
    while (p) {
        init_declarator_list_s_t *t = p;
        p = p->next;
        freeID(&t->idecl);
        delete t;
    }
}
