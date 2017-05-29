#include "util.h"
using namespace std;
extern map<string,Variable*> var_table;
extern map<string, Function*> func_table;
extern int Gvar_count;
extern int Var_count; //统计所有变量
extern vector<ins> com_ins;
extern map<string, unsigned> label_table;

void init_debug()
{
    for(auto it = func_table.begin(); it != func_table.end(); it++)
    {
        auto it_var = it->second;
        it_var->Print_Func();
    }
    for(auto it = var_table.begin(); it != var_table.end(); it++)
    {
        auto it_var = it->second;
        it_var->Print_Var();
    }
    for(auto it = com_ins.begin()+1; it != com_ins.end(); it++)
    {
        it->Print_Ins();
    }
}

std::string fix_name(std::string var_name, Function *now_fun) //这个函数不检查合法性，var_name 也有可能是一个数字
{
    if(var_name[0] == 'p')
        return extend_p_name(var_name, now_fun -> f_name);
    else return var_name;
}

void checkNotArray(Variable *this_var)
{
    if(this_var -> isArray == 1)
        yyerror("Array can not be changed!");
}

void checkArray(Variable *this_var)
{
    if(this_var -> isArray == 0)
        yyerror("In statement like t1[t2], t1 must be array!!");
}