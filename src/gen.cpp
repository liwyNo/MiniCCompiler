#include "gen.h"
#include <cstdio>
#include <cstring>

int now_byte = 0;
FILE *file_p;
void set_output(const char *s)
{
    file_p = fopen(s, "wb");
}
/*
int count_byte(int x)
{
    int cnt = 0;
    while (x)
    {
        cnt++;
        x /= 10;
    }
    return cnt;
}
*/
void gen_func(int num)
{
    now_byte += fprintf(file_p, "f%d:\n", num);
    //now_byte += 3 + count_byte(num);
}

void gen_label(int num)
{
    now_byte += fprintf(file_p, "l%d\n", num);
    //now_byte += 3 + count_byte(num);
}

void gen_const(const char *type, const char *name, const void *value)
{
    //#warning "this function has not been implemented"
    if (strcmp(type, "int1"))
    {
        char *p = (char *)value;
        now_byte += fprintf(file_p, "const %s %s %d\n", type, name, *p);
    }
    if (strcmp(type, "int2"))
    {
        short int *p = (short int *)value;
        now_byte += fprintf(file_p, "const %s %s %d\n", type, name, *p);
    }
    if (strcmp(type, "int4"))
    {
        int *p = (int *)value;
        now_byte += fprintf(file_p, "const %s %s %d\n", type, name, *p);
    }
    if (strcmp(type, "int8"))
    {
        long long *p = (long long *)value;
        now_byte += fprintf(file_p, "const %s %s %lld\n", type, name, *p);
    }
    if (strcmp(type, "float4"))
    {
        float *p = (float *)value;
        now_byte += fprintf(file_p, "const %s %s %f\n", type, name, *p);
    }
    if (strcmp(type, "float8"))
    {
        double *p = (double *)value;
        now_byte += fprintf(file_p, "const %s %s %lf\n", type, name, *p);
    }
    if (strcmp(type, "pointer"))
    {
        const void *p = value;
        //#warning "how to print pointer?"
        now_byte += fprintf(file_p, "const %s %s %p\n", type, name, p);
    }
    if (strcmp(type, "str"))
    {
        char *p = (char *)value;
        int n = strlen(p);
        now_byte += fprintf(file_p, "const %s %s", type, name); 
        while(*p!='\0')
            now_byte += fprintf(file_p, " %d",*p);
        now_byte += fprintf(file_p, " 0\n");
    }
}

void gen_var(const char *type, const char *name, int length)
{
    if (length == -1)
    {
        now_byte += fprintf(file_p, "var %s %s\n", type, name);
        //now_byte + 6 +strlen(type)+strlen(name);
    }
    else
    {
        now_byte += fprintf(file_p, "var %s %d %s\n", type, length, name);
        //now_byte + 7 +strlen(type)+strlen(name)+count_byte(length);
    }
}

void gen_gval(const char *type, const char *name, int length)
{
    if (length == -1)
    {
        now_byte += fprintf(file_p, "gvar %s %s\n", type, name);
        //now_byte + 7 +strlen(type)+strlen(name);
    }
    else
    {
        now_byte += fprintf(file_p, "gvar %s %d %s\n", type, length, name);
        //now_byte += 8 +strlen(type)+strlen(name)+count_byte(length);
    }
}

void gen_op2(const char *name1, const char *name2, const char *name3, const char *op)
{
    now_byte += fprintf(file_p, "%s = %s %s %s\n", name1, name2, op, name3);
}

void gen_op1(const char *name1, const char *name2, const char *op)
{
    now_byte += fprintf(file_p, "%s = %s %s\n", name1, op, name2);
}

void gen_cast(const char *name1, const char *name2, const char *type)
{
    now_byte += fprintf(file_p, "%s = %s %s\n", name1, type, name2);
}

void gen_cpy(const char *name1, const char *name2)
{
    now_byte += fprintf(file_p, "%s = %s\n", name1, name2);
}

void gen_arr_cpy(const char *name1, const char *name2, const char *name3)
{
    now_byte += fprintf(file_p, "%s [ %s ] = %s\n", name1, name2, name3);
}

void gen_pnt_cpy(const char *name1, const char *name2)
{
    now_byte += fprintf(file_p, "* %s = %s\n", name1, name2);
}

void gen_cpy_arr(const char *name1, const char *name2, const char *name3)
{
    now_byte += fprintf(file_p, "%s = %s [ %s ]\n", name1, name2, name3);
}

void gen_goto(int num)
{
    now_byte += fprintf(file_p, "goto l%d\n", num);
}

int gen_b_goto()
{
    now_byte += fprintf(file_p, "goto l     \n"); //留了5个空格
    return now_byte - 5;
}

void gen_if_goto(const char *name1, const char *name2, const char *op, int num)
{
    now_byte += fprintf(file_p, "if %s %s %s goto l%d\n", name1, op, name2, num);
}

int gen_if_b_goto(const char *name1, const char *name2, const char *op)
{
    now_byte += fprintf(file_p, "if %s %s %s goto l     \n", name1, op, name2);
    return now_byte - 5;
}

void gen_param(const char *name)
{
    now_byte += fprintf(file_p, "param %s\n", name);
}

void gen_call(int num, int pnum)
{
    now_byte += fprintf(file_p, "call f%d %d\n", num, pnum);
}

void gen_cpy_call(const char *name, int num, int pnum)
{
    now_byte += fprintf(file_p, "%s = call f%d %d\n", name, num, pnum);
}

void gen_return(const char *name)
{
    if (name)
        now_byte += fprintf(file_p, "return %s\n", name);
    else
        now_byte += fprintf(file_p, "return\n");
}
