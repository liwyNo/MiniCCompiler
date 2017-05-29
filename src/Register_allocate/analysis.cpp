#include "analysis.h"
using namespace std;

extern vector<ins*> com_ins;
extern map<string, unsigned> label_table;
extern map<string,Variable*> var_table;
extern map<string, Function*> func_table;

int __get_Lable(string l_name)
{
    if(label_table.find(l_name) == label_table.end())
        yyerror("Label not found!");
    else return label_table[l_name];
}

void init_preI()//假装是正确的了，不管正确性验证了
{
    int i;
    //int isGlobal = 1;
    for (i = 0; i < com_ins.size(); i++)
    {
        ins *now_ins = com_ins[i];
        if(now_ins->type == iVAR || now_ins->type == iGVAR) //只有声明语句可能在外面，这个我根本不用管
            continue;
        if(now_ins->type == iFBEGIN) //函数开头这种语句是没有上一句的，也不管
            continue;
        //只有 label 语句可能有多种前置语句，其他的都是上一句
        now_ins -> preI.push_back(com_ins[i-1]);
        //以下是两种可能走到其他语句的语句
        if(now_ins->type == iIF)
        {
            int lab = __get_Lable(now_ins -> arg4);
            com_ins[lab]->preI.push_back(now_ins);
        }
        if(now_ins->type == iGOTO)
        {
            int lab = __get_Lable(now_ins -> arg1);
            com_ins[lab]->preI.push_back(now_ins);
        }
    }
}