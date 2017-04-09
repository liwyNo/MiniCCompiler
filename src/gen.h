#ifndef GEN_H
#define GEN_H
extern int now_byte;
void set_output(const char *s);

//int count_byte(int x);

void gen_func(/*int num, */const char *name);

void gen_label(int num);

void gen_const(const char *type, const char *name, const void *value);

void gen_var(const char *type, const char *name, int length = -1);

void gen_gvar(const char *type, const char *name, int length = -1);

void gen_op2(const char *name1, const char *name2, const char *name3, const char *op);

void gen_op1(const char *name1, const char *name2, const char *op);

void gen_cast(const char *name1, const char *name2, const char *type);

void gen_cpy(const char *name1, const char *name2);

void gen_arr_cpy(const char *name1, const char *name2, const char *name3);

void gen_pnt_cpy(const char *name1, const char *name2);

void gen_cpy_arr(const char *name1, const char *name2, const char *name3);

void gen_goto(int num);

//int gen_b_goto(); //留了5个空格，设返回的值为x,则x恰好为第一个空格的位置

void gen_if_goto(const char *name1, const char *name2, const char *op, int num);

//int gen_if_b_goto(const char *name1, const char *name2, const char *op); //留了5个空格，设返回的值为x,则x恰好为第一个空格的位置

void gen_param(const char *name);

void gen_call(char *f_name, int pnum);

void gen_cpy_call(const char *name, char *f_name, int pnum);

void gen_return(const char *name); // name是NULL时表示没有返回值

#endif
