#include "typedef.h"
#include "Eeyore.tab.hpp"
using namespace std;
map<string,Variable*> var_table;
map<string, int> label_table; //直接表示这个label的行号（指令号）
map<string, Function*> func_table;
int Gvar_count;

Function::Function(string _name, int _num):f_name(_name),arg_num(_num) { }

Function* new_Function(string fun_name, int arg_num)
{
    if(get_Fun(fun_name) == nullptr)
    {
        Function* nf= new Function(fun_name, arg_num);
        func_table[fun_name]=nf;
        return nf;
    }
    else yyerror("Function can't be declared twice!");
}

Function* get_Fun(string fun_name)
{
    if(func_table.find(fun_name)!=func_table.end())
        return func_table[fun_name];
    else return nullptr;
}

Variable* new_Var(string var_name, int isGlobal)
{
    ;
}

Variable* get_Var(string var_name)
{
    if(var_table.find(var_name)!=var_table.end())
        return var_table[var_name];
    else return nullptr;
}