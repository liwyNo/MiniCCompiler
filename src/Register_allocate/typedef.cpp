#include "typedef.h"
#include "Eeyore.tab.hpp"
using namespace std;
map<string,Variable*> var_table;
map<string, int> label_table; //直接表示这个label的行号（指令号）
map<string, Function*> func_table;
int Gvar_count;

std::string extend_p_name(std::string p_name, std::string f_name)
{
    return p_name + "_" + f_name; //后面接上所在函数的名字，这样就独一无二了
}
Function::Function(string _name, int _num):f_name(_name),arg_num(_num) { 
    //把它的所有参数都先声明出来
    for (int i = 0; i < arg_num; i++)
        new_Var(extend_p_name("p_"+to_string(i), f_name),0, this);
}
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
string __get_new_gvar_name()
{
    return "v"+to_string(Gvar_count);
} 
Variable::Variable(bool _isGlobal, std::string _s_name, bool _isArray):isGlobal(_isGlobal),s_name(_s_name),isArray(_isArray) {}
Variable* new_Var(string var_name, int isGlobal, Function* now_fun = nullptr)
{
    if(get_Var(var_name) != nullptr)
        yyerror("Variable can't be declared twice!");
    Variable* nv = new Variable(isGlobal, var_name, 0);
    
    if(isGlobal)
    {
        Gvar_count ++;
        nv -> v_name = __get_new_gvar_name();
    }
    else
    {
        nv -> spill_loc = now_fun -> stack_size; //栈空间下标从0开始
        nv -> fa_func = now_fun;
        now_fun -> stack_size ++;
        now_fun -> local_var.push_back(nv);
    }
    var_table[var_name] = nv;
    return nv;
}

Variable* new_Var_Arr(string var_name, int isGlobal, int size, Function* now_fun = nullptr)
{
    if(get_Var(var_name) != nullptr)
        yyerror("Variable can't be declared twice!");
    Variable* nv = new Variable(isGlobal, var_name, 1);
    nv -> arr_size = size;
    if(isGlobal)
    {
        Gvar_count ++;
        nv -> v_name = __get_new_gvar_name();
    }
    else
    {
        nv -> spill_loc = now_fun -> stack_size; //栈空间下标从0开始
        now_fun -> stack_size += size;
    }
    var_table[var_name] = nv;
    return nv;
}

Variable* get_Var(string var_name)
{
    if(var_table.find(var_name)!=var_table.end())
        return var_table[var_name];
    else return nullptr;
}

Variable* get_Var_in_Func(std::string var_name, Function *now_fun) //在 now_fun 里面返回 var_name 这个变量，如果没有，或者作用域不对，则返回 nullptr
{
    Variable* rel = get_Var(var_name);
    if(rel -> isGlobal || rel -> fa_func == now_fun)
        return rel;
    else 
        return nullptr;
}