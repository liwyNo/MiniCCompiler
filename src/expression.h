#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "symbol.h"
#include "yaccType.h"
extern const IdType_t type_to_type[IDTYPE_NUM][IDTYPE_NUM];

char *sizeof_type(const_Typename_ptr b_type);//得到这个type的类型的大小，返回一个三地址码中的c型常量的名字,顺便判了void
void postfix_expression_INC_DEC_OP(expression_s_t &This, const expression_s_t &Next, const char *op);
#endif