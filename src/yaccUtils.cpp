#include "yaccUtils.h"
#include "gen.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

void genDeclare(const Identifier_t *id, bool global)
{
    auto autogen = global ? gen_gval : gen_var;
    switch (id->type->type) {
        case idt_char:
            autogen("int1", id->TACname, -1);
            break;
        case idt_short:
            autogen("int2", id->TACname, -1);
            break;
        case idt_int:
            autogen("int4", id->TACname, -1);
            break;
        case idt_long:
            autogen("int8", id->TACname, -1);
            break;
        case idt_uchar:
            autogen("uint1", id->TACname, -1);
            break;
        case idt_ushort:
            autogen("uint2", id->TACname, -1);
            break;
        case idt_uint:
            autogen("uint4", id->TACname, -1);
            break;
        case idt_ulong:
            autogen("uint8", id->TACname, -1);
            break;
        case idt_float:
            autogen("float4", id->TACname, -1);
            break;
        case idt_double:
            autogen("float8", id->TACname, -1);
            break;
        case idt_pointer:
            autogen("ptr", id->TACname, -1);
            break;
        case idt_fpointer:
            break;
        case idt_void:
            yyerror("declaration of void type");
        case idt_array:
            autogen("ptr", id->TACname, id->type->size);
            break;
        case idt_struct:
            autogen("ptr", id->TACname, id->type->size);
            break;
        case idt_union:
            autogen("ptr", id->TACname, id->type->size);
            break;
        case idt_enum:
            autogen("uint4", id->TACname, -1);
            break;
    }
}
