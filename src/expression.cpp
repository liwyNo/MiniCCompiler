#include "expression.h"
/*
typedef enum {
    idt_none, 
    idt_char, idt_short, idt_int, idt_long, 
    idt_uchar, idt_ushort, idt_uint, idt_ulong, 
    idt_float, idt_double, 
    idt_pointer, idt_fpointer, 
    idt_void, 
    idt_array, 
    idt_struct, idt_union, 
    idt_enum 
} IdType_t;
*/

//利用了外部的type_to_type.cpp打的表，生成规则详见type_to_type.cpp
const IdType_t type_to_type[IDTYPE_NUM][IDTYPE_NUM]=
{{(IdType_t)0,(IdType_t)2,(IdType_t)2,(IdType_t)3,(IdType_t)2,(IdType_t)2,(IdType_t)6,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)2,(IdType_t)1,(IdType_t)2,(IdType_t)3,(IdType_t)2,(IdType_t)2,(IdType_t)6,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)2,(IdType_t)2,(IdType_t)2,(IdType_t)3,(IdType_t)2,(IdType_t)2,(IdType_t)6,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)3,(IdType_t)3,(IdType_t)3,(IdType_t)3,(IdType_t)3,(IdType_t)3,(IdType_t)3,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)2,(IdType_t)2,(IdType_t)2,(IdType_t)3,(IdType_t)4,(IdType_t)2,(IdType_t)6,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)2,(IdType_t)2,(IdType_t)2,(IdType_t)3,(IdType_t)2,(IdType_t)5,(IdType_t)6,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)6,(IdType_t)6,(IdType_t)6,(IdType_t)3,(IdType_t)6,(IdType_t)6,(IdType_t)6,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)7,(IdType_t)7,(IdType_t)7,(IdType_t)7,(IdType_t)7,(IdType_t)7,(IdType_t)7,(IdType_t)7,(IdType_t)8,(IdType_t)9,(IdType_t)10,(IdType_t)11,(IdType_t)-1,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)8,(IdType_t)9,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)9,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)10,(IdType_t)10,(IdType_t)10,(IdType_t)10,(IdType_t)10,(IdType_t)10,(IdType_t)10,(IdType_t)10,(IdType_t)-1,(IdType_t)-1,(IdType_t)2,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)11,(IdType_t)11,(IdType_t)11,(IdType_t)11,(IdType_t)11,(IdType_t)11,(IdType_t)11,(IdType_t)11,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)2,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)13,(IdType_t)13,(IdType_t)13,(IdType_t)13,(IdType_t)13,(IdType_t)13,(IdType_t)13,(IdType_t)13,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)2,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1},
{(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1,(IdType_t)-1}
};

