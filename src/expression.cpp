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
        if (Next.type->type < 10 || Next.type->type == idt_pointer) //数字 or pointer
        {
            char *tmp_name = get_TAC_name('t', CreateTempVar()), *addr_1 = Next.get_addr();
            gen_var(map_name[Next.type->type], tmp_name);
            gen_cpy(tmp_name, addr_1);
            if (Next.type->type < 8) //number!
            {
                char *int_1 = get_cast_name(Next.type->type, idt_int, "c1"); //永远要消息变量类型要一样
                gen_op2(addr_1, addr_1, int_1, op);
            }
            else //pointer
            {
                const_Typename_ptr b_type = (((Next.type)->structure)->pointer).base_type;
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

expression_s_t __Assign(expression_s_t &A, const expression_s_t &B) //不加类型合法行判断的直接赋值。。。
{
    char *addr_b = B.get_addr(), *addr_rel;
    addr_rel = get_cast_name(A.type->type, B.type->type, addr_b);
    if (A.addr == NULL)
        gen_pnt_cpy(A.laddr, addr_b);
    else
        gen_cpy(A.get_addr(), addr_rel);
    return A;
}

expression_s_t get_assign(expression_s_t &A, const expression_s_t &B)
{
    if (A.lr_value == 1)
        yyerror("lvalue required as left operand of assignment!");
    if (A.type->isConst == 1)
        yyerror("assignment of read-only variable!");
    if (A.type->type < 10) //数字可以赋成数字和指针和array和函数指针。然而在c++中，就只有数字可以！注意:不存在enum类型的变量！
    {
        if (B.type->type < 12 || B.type->type == idt_array)
        {
            return __Assign(A, B);
        }
        else
            yyerror("number can only be assigned with a number!");
    }
    if (A.type->type == idt_pointer || A.type->type == idt_fpointer) //指针之间能随便复制，但是减法只有同类型之间可以。指针还能等于整数！类型用sameType函数判断！
    {
        if (B.type->type == idt_pointer || B.type->type == idt_fpointer || B.type->type == idt_array)
        {
            return __Assign(A, B);
        }
        else if (B.type->type < 8)
        {
            return __Assign(A, B);
        }
        else
            yyerror("pointer and fpointer can only be assigned with a pointer/fpointer/array");
    }
    if (A.type->type == idt_struct || A.type->type == idt_union)
    {
#warning "haven't implement the assignment of struct'"
    }
    //array显然是右值，不用考虑
}

const_Typename_ptr get_Typename_t(IdType_t type)
{
    if(type >= 10)
        yyerror("this function only use for nubmer!");
    if(type == idt_char)
        return (const_Typename_ptr)LookupSymbol("char", NULL);
    if(type == idt_uchar)
        return (const_Typename_ptr)LookupSymbol("unsigned char", NULL);
    if(type == idt_short)
        return (const_Typename_ptr)LookupSymbol("short int", NULL);
    if(type == idt_ushort)
        return (const_Typename_ptr)LookupSymbol("unsigned short int", NULL);
    if(type == idt_int)
        return (const_Typename_ptr)LookupSymbol("int", NULL);
    if(type == idt_uint)
        return (const_Typename_ptr)LookupSymbol("unsigned int", NULL);
    if(type == idt_long)
        return (const_Typename_ptr)LookupSymbol("long long", NULL);
    if(type == idt_ulong)
        return (const_Typename_ptr)LookupSymbol("unsigned long long", NULL);
    if(type == idt_float)
        return (const_Typename_ptr)LookupSymbol("float", NULL);
    if(type == idt_double)
        return (const_Typename_ptr)LookupSymbol("double", NULL);
}

void get_ADD_SUB_MUL_DIV(expression_s_t &This, const expression_s_t &A, const expression_s_t &B, const char *op) //处理加减乘除，答案放This里
{
    if (A.type->type < 10 && B.type->type < 10)
    {
        IdType_t rel_type = type_to_type[A.type->type][B.type->type];
        char *val_a, *val_b;
        val_a = get_cast_name(rel_type, A.type->type, A.get_addr());
        val_b = get_cast_name(rel_type, B.type->type, B.get_addr());
        char *rel = get_TAC_name('t', CreateTempVar());
        gen_var(map_name[rel_type], rel);
        gen_op2(rel,val_a,val_b,op);
        This.type = get_Typename_t(rel_type);
        This.addr = rel;
    }
    else if (op[0] == '-') //同类型指针之间可以减法,函数指针之间和void*指针之间的减法就是绝对地址的减法，其他指针的减法要除以单位大小！
    {
        if (A.type->type == idt_array || A.type->type == idt_pointer || A.type->type == idt_fpointer)
            if (B.type->type == idt_array || B.type->type == idt_pointer || B.type->type == idt_fpointer)
                if (sameType(A.type, B.type))
                {
                    char *rel = get_TAC_name('t', CreateTempVar());
                    gen_var("int4", rel);
                    gen_op2(rel, A.get_addr(), B.get_addr(), "-");
                    if (A.type->type == idt_fpointer || (A.type->type == idt_pointer && A.type->structure->pointer.base_type -> type == idt_void))
                        ; //nothing to do
                    else  //否则需要除以单位大小
                    {
                        const_Typename_ptr b_type = A.type->structure->pointer.base_type;
                        char *b_size = sizeof_type(b_type);
                        gen_op2(rel, rel, b_size, "/");
                    }
                    This.type = (const_Typename_ptr)LookupSymbol("int", NULL);
                    This.addr = rel;
                }
                else
                    yyerror("only same type of pointer can subtract");
    }
    else
        yyerror("only numbers or same type of pointers can subtract");
    This.isConst = A.isConst & B.isConst;
    #warning ""haven't implement the calculation of const"
    if(This.isConst == 1)
        ;
    This.lr_value = 1;
    This.laddr = NULL;
}