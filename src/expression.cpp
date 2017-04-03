#include "expression.h"
#include "MiniC.tab.hpp"
#include "gen.h"
#include "yaccUtils.h"
#include <cstring>
#include <string>
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
const IdType_t type_to_type[IDTYPE_NUM][IDTYPE_NUM] =
    {{(IdType_t)0, (IdType_t)2, (IdType_t)2, (IdType_t)3, (IdType_t)2, (IdType_t)2, (IdType_t)6, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)2, (IdType_t)1, (IdType_t)2, (IdType_t)3, (IdType_t)2, (IdType_t)2, (IdType_t)6, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)2, (IdType_t)2, (IdType_t)2, (IdType_t)3, (IdType_t)2, (IdType_t)2, (IdType_t)6, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)3, (IdType_t)3, (IdType_t)3, (IdType_t)3, (IdType_t)3, (IdType_t)3, (IdType_t)3, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)2, (IdType_t)2, (IdType_t)2, (IdType_t)3, (IdType_t)4, (IdType_t)2, (IdType_t)6, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)2, (IdType_t)2, (IdType_t)2, (IdType_t)3, (IdType_t)2, (IdType_t)5, (IdType_t)6, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)6, (IdType_t)6, (IdType_t)6, (IdType_t)3, (IdType_t)6, (IdType_t)6, (IdType_t)6, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)7, (IdType_t)7, (IdType_t)7, (IdType_t)7, (IdType_t)7, (IdType_t)7, (IdType_t)7, (IdType_t)7, (IdType_t)8, (IdType_t)9, (IdType_t)10, (IdType_t)11, (IdType_t)-1, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)8, (IdType_t)9, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)9, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)10, (IdType_t)10, (IdType_t)10, (IdType_t)10, (IdType_t)10, (IdType_t)10, (IdType_t)10, (IdType_t)10, (IdType_t)-1, (IdType_t)-1, (IdType_t)2, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)11, (IdType_t)11, (IdType_t)11, (IdType_t)11, (IdType_t)11, (IdType_t)11, (IdType_t)11, (IdType_t)11, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)2, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)13, (IdType_t)13, (IdType_t)13, (IdType_t)13, (IdType_t)13, (IdType_t)13, (IdType_t)13, (IdType_t)13, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)2, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1},
     {(IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1, (IdType_t)-1}};

char *sizeof_type(const_Typename_ptr b_type) //得到这个type的类型的大小，返回一个三地址码中的c型常量的名字
{
    if (b_type->type == idt_void) // void* 指针不能这么搞
        yyerror("b_type -> type should not be void!");
    char *c_name = get_TAC_name('c', CreateConstant());
    gen_const("int4", c_name, &(b_type->size));
    return c_name;
}

void postfix_expression_INC_DEC_OP(expression_s_t &This, const expression_s_t &Next, const char *op)
{
    if (Next.lr_value == 0)
    {
        if (Next.type->type <= 8) //数字 or pointer
        {
            char *tmp_name = get_TAC_name('t', CreateTempVar()), *addr_1 = Next.get_addr();
            gen_var(map_name[Next.type->type], tmp_name);
            gen_cpy(tmp_name, addr_1);
            if (Next.type->type < 8) //number!
                gen_op2(addr_1, addr_1, "c1", op);
            else //pointer
            {
                const_Typename_ptr b_type = (((Next.type) -> structure)->pointer).base_type;
                char *b_size = sizeof_type(b_type);
                gen_op2(addr_1, addr_1, b_size, op);
            }
            if (Next.addr == NULL) // 是用地址访问的，就需要设置laddr,否则不需要设置
                gen_pnt_cpy(Next.laddr, addr_1);
            This.lr_value = 1; //i++ 是右值，++i是左值
            This.isConst = 0;
            This.type = Next.type;
            This.addr = tmp_name;
            This.laddr = NULL;
        }
        else
            yyerror("you can only use ++ on number or pointer");
    }
    else
        yyerror("Can't use ++ operator on right value!");
}