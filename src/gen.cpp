#include "gen.h"
#include <cstdio>
#include <cstring>

/*int now_byte = 0;
FILE *file_p;
void set_output(const char *s)
{
    file_p = fopen(s, "wb");
}*/
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
    printf("f%d:\n", num);
    //now_byte += 3 + count_byte(num);
}

void gen_label(int num)
{
    printf("l%d:\n", num);
    //now_byte += 3 + count_byte(num);
}

void gen_const(const char *type, const char *name, const void *value)
{
    if (strcmp(type, "int1")==0)
    {
        char *p = (char *)value;
        printf("const %s %s %hhd\n", type, name, *p);
    }
    else if (strcmp(type, "int2")==0)
    {
        short int *p = (short int *)value;
        printf("const %s %s %hd\n", type, name, *p);
    }
    else if (strcmp(type, "int4")==0)
    {
        int *p = (int *)value;
        printf("const %s %s %d\n", type, name, *p);
    }
    else if (strcmp(type, "int8")==0)
    {
        long long *p = (long long *)value;
        printf("const %s %s %lld\n", type, name, *p);
    }
    else if (strcmp(type, "uint1")==0)
    {
        unsigned char *p = (unsigned char *)value;
        printf("const %s %s %hhu\n", type, name, *p);
    }
    else if (strcmp(type, "uint2")==0)
    {
        unsigned short int *p = (unsigned short int *)value;
        printf("const %s %s %hu\n", type, name, *p);
    }
    else if (strcmp(type, "uint4")==0)
    {
        unsigned int *p = (unsigned int *)value;
        printf("const %s %s %u\n", type, name, *p);
    }
    else if (strcmp(type, "uint8")==0)
    {
        unsigned long long *p = (unsigned long long *)value;
        printf("const %s %s %llu\n", type, name, *p);
    }
    else if (strcmp(type, "float4")==0)
    {
        float *p = (float *)value;
        printf("const %s %s %f\n", type, name, *p);
    }
    else if (strcmp(type, "float8")==0)
    {
        double *p = (double *)value;
        printf("const %s %s %lf\n", type, name, *p);
    }
    else if (strcmp(type, "ptr")==0)
    {
        const void *p = value;
        //#warning "how to print pointer?"
        printf("const %s %s %p\n", type, name, p);
    }
    else if (strcmp(type, "str")==0)
    {
        char *p = (char *)value;
        int n = strlen(p);
        printf("const %s %s", type, name); 
        while(*p!='\0')
            printf(" %d",*(p++));
        printf(" 0\n");
    }
}

void gen_var(const char *type, const char *name, int length)
{
    if (length == -1)
    {
        printf("var %s %s\n", type, name);
        //now_byte + 6 +strlen(type)+strlen(name);
    }
    else
    {
        printf("var %s %d %s\n", type, length, name);
        //now_byte + 7 +strlen(type)+strlen(name)+count_byte(length);
    }
}

void gen_gvar(const char *type, const char *name, int length)
{
    if (length == -1)
    {
        printf("gvar %s %s\n", type, name);
        //now_byte + 7 +strlen(type)+strlen(name);
    }
    else
    {
        printf("gvar %s %d %s\n", type, length, name);
        //now_byte += 8 +strlen(type)+strlen(name)+count_byte(length);
    }
}

void gen_op2(const char *name1, const char *name2, const char *name3, const char *op)
{
    printf("%s = %s %s %s\n", name1, name2, op, name3);
}

void gen_op1(const char *name1, const char *name2, const char *op)
{
    printf("%s = %s %s\n", name1, op, name2);
}

void gen_cast(const char *name1, const char *name2, const char *type)
{
    printf("%s = %s %s\n", name1, type, name2);
}

void gen_cpy(const char *name1, const char *name2)
{
    printf("%s = %s\n", name1, name2);
}

void gen_arr_cpy(const char *name1, const char *name2, const char *name3)
{
    printf("%s [ %s ] = %s\n", name1, name2, name3);
}

void gen_pnt_cpy(const char *name1, const char *name2)
{
    printf("* %s = %s\n", name1, name2);
}

void gen_cpy_arr(const char *name1, const char *name2, const char *name3)
{
    printf("%s = %s [ %s ]\n", name1, name2, name3);
}

void gen_goto(int num)
{
    printf("goto l%d\n", num);
}
/*
int gen_b_goto()
{
    printf("goto l     \n"); //留了5个空格
    return now_byte - 5;
}*/

void gen_if_goto(const char *name1, const char *name2, const char *op, int num)
{
    printf("if %s %s %s goto l%d\n", name1, op, name2, num);
}
/*
int gen_if_b_goto(const char *name1, const char *name2, const char *op)
{
    printf("if %s %s %s goto l     \n", name1, op, name2);
    return now_byte - 5;
}*/

void gen_param(const char *name)
{
    printf("param %s\n", name);
}

void gen_call(char *f_name, int pnum)
{
    printf("call %s %d\n", f_name, pnum);
}

void gen_cpy_call(const char *name, char *f_name, int pnum)
{
    printf("%s = call %s %d\n", name, f_name, pnum);
}

void gen_return(const char *name)
{
    if (name)
        printf("return %s\n", name);
    else
        printf("return\n");
}
