#include "yaccUtils.h"
#include "gen.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

Typename_t *newEnum(char *name, EnumTable_t *p)
{
    Typename_t *t = new Typename_t;
    t->type = idt_enum;
    t->size = 4;
    t->name = NULL;
    t->structure = new IdStructure_t;
    t->structure->enumerate = p;
    StackAddTypename(t);
    return t;
}

void genDeclare(const_Typename_ptr type, const char *TACname, bool global)
{
    auto autogen = global ? gen_gval : gen_var;
    switch (type->type) {
    case idt_char:
        autogen("int1", TACname, -1);
        break;
    case idt_short:
        autogen("int2", TACname, -1);
        break;
    case idt_int:
        autogen("int4", TACname, -1);
        break;
    case idt_long:
        autogen("int8", TACname, -1);
        break;
    case idt_uchar:
        autogen("uint1", TACname, -1);
        break;
    case idt_ushort:
        autogen("uint2", TACname, -1);
        break;
    case idt_uint:
        autogen("uint4", TACname, -1);
        break;
    case idt_ulong:
        autogen("uint8", TACname, -1);
        break;
    case idt_float:
        autogen("float4", TACname, -1);
        break;
    case idt_double:
        autogen("float8", TACname, -1);
        break;
    case idt_pointer:
        autogen("ptr", TACname, -1);
        break;
    case idt_fpointer:
        break;
    case idt_void:
        yyerror("declaration of void type");
    case idt_array:
        autogen("ptr", TACname, type->size);
        break;
    case idt_struct:
        autogen("ptr", TACname, type->size);
        break;
    case idt_union:
        autogen("ptr", TACname, type->size);
        break;
    case idt_enum:
        autogen("uint4", TACname, -1);
        break;
    }
}

void genInitilize(const_Typename_ptr type, const char *TACname, const initializer_s_t *init)
{
    if (init == NULL)
        return;
    if (type == NULL || TACname == NULL)
        yyerror("no id for initilization");
    if (init->addr) {
#warning "call operator="
        printf("(!!) %s = %s\n", TACname, init->addr);
    }
    else {
        int tnum = CreateTempVar();
        char *tname = strdup(('t' + std::to_string(tnum)).c_str());
        genDeclare(type, tname, false);
        gen_cpy(tname, TACname);
        std::vector<initializer_s_t> vinit;
        for (initializer_list_s_t *i = init->lst; i; i = i->next)
            vinit.push_back(i->data);
        auto init_it = vinit.rbegin();
        initializer_s_t noinit;
        noinit.lst = NULL;
        char c0[] = "c0";
        if (type->type == idt_array) {
            const_Typename_ptr btp = type->structure->pointer.base_type;
            int csize = CreateConstant();
            std::string scsize = 'c' + std::to_string(csize);
            gen_const("int4", scsize.c_str(), &btp->size);
            for (int i = 0; i < type->structure->pointer.length; ++i) {
                if (init_it == vinit.rend()) {
                    noinit.addr = (btp->type == idt_struct || btp->type == idt_array ? NULL : c0);
                    genInitilize(btp, tname, &noinit);
                }
                else
                    genInitilize(btp, tname, &*(init_it++));
                gen_op2(tname, tname, scsize.c_str(), "+");
            }
        }
        else if (type->type == idt_struct) {
            std::vector<SymbolList_t*> vsl;
            for (SymbolList_t *i = type->structure->record; i; i = i->next)
                vsl.push_back(i);
            for (auto vsl_it = vsl.rbegin(); vsl_it != vsl.rend(); ++vsl_it) {
                int csize = CreateConstant();
                std::string scsize = 'c' + std::to_string(csize);
                gen_const("int4", scsize.c_str(), &(*vsl_it)->offset);
                gen_op2(tname, TACname, scsize.c_str(), "+");
                const_Typename_ptr btp = (*vsl_it)->id->type;
                if (init_it == vinit.rend()) {
                    noinit.addr = (btp->type == idt_struct || btp->type == idt_array ? NULL : c0);
                    genInitilize(btp, tname, &noinit);
                }
                else
                    genInitilize(btp, tname, &*(init_it++));
            }
        }
        else
            yyerror("initilize error");
    }
}
