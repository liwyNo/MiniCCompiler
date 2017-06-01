#include "typedef.h"
#include "Eeyore.tab.hpp"
#include <iostream>
using namespace std;
#define debug(x) cerr<<#x<<"="<<x<<endl
map<string,Variable*> var_table;
Variable * num_to_var[1000];
map<string, Function*> func_table;
int Gvar_count;
int Var_count; //统计所有变量

std::string extend_p_name(std::string p_name, std::string f_name)
{
    return p_name + "_" + f_name; //后面接上所在函数的名字，这样就独一无二了
}
Function::Function(string _name, int _num):f_name(_name),arg_num(_num) { 
    //把它的所有参数都先声明出来
    stack_size = 12; //先把12个callee save的寄存器的空间存下来
    for (int i = 0; i < arg_num; i++)
        new_Var(extend_p_name("p"+to_string(i), f_name),0, this);
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
Variable::Variable(bool _isGlobal, std::string _s_name, bool _isArray):isGlobal(_isGlobal),s_name(_s_name),isArray(_isArray) 
{
    reg = nullptr;
    inMemory == _isGlobal;
}
Variable* new_Var(string var_name, int isGlobal, Function* now_fun = nullptr)
{
    //debug(var_name);    
    if(get_Var(var_name) != nullptr)
        yyerror("Variable can't be declared twice!");
    Variable* nv = new Variable(isGlobal, var_name, 0);
    Var_count ++;
    //debug(Var_count);
    //debug(var_name);
    //debug(isGlobal);
    nv -> num = Var_count;
    
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
    num_to_var[Var_count] = nv;
    //debug("end new_var");
    return nv;
}

Variable* new_Var_Arr(string var_name, int isGlobal, int size, Function* now_fun = nullptr)
{
    if(get_Var(var_name) != nullptr)
        yyerror("Variable can't be declared twice!");
    Variable* nv = new Variable(isGlobal, var_name, 1);
    Var_count ++;
    nv -> num = Var_count;
    nv -> arr_size = size;
    if(isGlobal)
    {
        Gvar_count ++;
        nv -> v_name = __get_new_gvar_name();
    }
    else
    {
        nv -> spill_loc = now_fun -> stack_size; //栈空间下标从0开始
        now_fun -> stack_size += size/4;  //别忘除4！！！
    }
    var_table[var_name] = nv;
    num_to_var[Var_count] = nv;
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
    if(rel == nullptr)
    {
        yyerror("Can not find the var in this function!'");
        return nullptr;
    }
    else if(rel -> isGlobal || rel -> fa_func == now_fun)
        return rel;
    else 
    {
        yyerror("Can not find the var in this function!'");
        return nullptr;
    }
}

void Variable::Print_Var()
{
    cerr << num << " " << s_name <<":"<< endl;
    debug(isGlobal);
    debug(isArray);
    debug(arr_size);    
    debug(v_name);
    debug(spill_loc);
    debug(fa_func);
    cerr << endl;
}

void Function::Print_Func()
{
    debug(f_name);
    debug(arg_num);
    debug(stack_size);
    cerr << endl;
}

void ins::Print_Ins()
{
    int i;
    debug(line_num);
    debug(type);
    debug(arg1);
    debug(arg2);
    debug(arg3);
    debug(arg4);
    for (i=1;i<=Var_count;i++)
        printf("%d",(int)def[i]);
    cout << endl;
    for (i=1;i<=Var_count;i++)
        printf("%d",(int)use[i]);
    cout << endl;
    for (i=1;i<=Var_count;i++)
        printf("%d",(int)live[i]);
    cout << endl;
    for(i=0;i<preI.size();i++)
        debug(preI[i]->line_num);
    cout<<endl;
}