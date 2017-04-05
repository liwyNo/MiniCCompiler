#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "symbol.h"
#include "yaccType.h"
extern const IdType_t type_to_type[IDTYPE_NUM][IDTYPE_NUM];

char *sizeof_type(const_Typename_ptr b_type);//得到这个type的类型的大小，返回一个三地址码中的c型常量的名字,顺便判了void
void postfix_expression_INC_DEC_OP(expression_s_t &This, const expression_s_t &Next, const char *op);
void INC_DEC_OP_unary_expression(expression_s_t &This, const char *op);
expression_s_t get_assign(expression_s_t &A, const expression_s_t &B); //处理赋值语句A=B，顺带检查合法性
void get_ADD_SUB_MUL_DIV(expression_s_t &This, const expression_s_t &A, const expression_s_t &B, const char *op);//处理加减乘除，答案放This里
void get_MOD_AND_OR_XOR_LEFT_RIGHT(expression_s_t &This, const expression_s_t &A, const expression_s_t &B, const char *op);//处理类似的只有整数之间进行的运算


#endif