#include "analysis.h"
#include <iostream>
#include <queue>
#include <algorithm>
using namespace std;
#define debug(x) cerr<<#x<<"="<<x<<endl

extern vector<ins> com_ins;
extern map<string, unsigned> label_table;
extern map<string,Variable*> var_table;
extern map<string, Function*> func_table;
extern int Var_count;
extern Variable* num_to_var[1000];
LiveInterval live_int[1000];

void LiveInterval::Print_Int()
{
    debug(var->s_name);
    debug(st);
    debug(ed);
    debug(spilled);
    debug(reg->r_name);
    cout << endl;
}

void Register::free()
{
    var = nullptr;
}

int __get_Lable(string l_name)
{
    if(label_table.find(l_name) == label_table.end())
    {
        yyerror("Label not found!");
        return -1;
    }
    else return label_table[l_name];
}

void init_preI()//假装是正确的了，不管正确性验证了
{
    int i;
    int isGlobal = 1;
    for (i = 1; i < com_ins.size(); i++)
    {
        //debug(i);
        ins *now_ins = &com_ins[i];
        //if(now_ins == nullptr)
            //debug("error");
        
        if(now_ins->type == iFBEGIN) //函数开头这种语句是没有上一句的，也不管
            isGlobal = 0;
        if(now_ins->type == iFEND)
            isGlobal = 1;
        //if(now_ins->type == iVAR || now_ins->type == iGVAR) //只有声明语句可能在外面，这个我根本不用管
            //continue;
        if(isGlobal || now_ins->type == iFBEGIN) //iFEND 作为边界起点，还是保留比较好
            continue;
        //只有 label 语句可能有多种前置语句，其他的都是上一句
        now_ins -> preI.push_back(&com_ins[i-1]);
        //以下是两种可能走到其他语句的语句
        if(now_ins->type == iIF)
        {
            int lab = __get_Lable(now_ins -> arg4);
            com_ins[lab].preI.push_back(now_ins);
        }
        if(now_ins->type == iGOTO)
        {
            int lab = __get_Lable(now_ins -> arg1);
            com_ins[lab].preI.push_back(now_ins);
        }
    }
}

void LiveVariableAnalysis() //类似 spfa 的方式进行迭代，找不动点
{
    queue<ins*> q;
    bool isGlobal = 1;
    for (auto it = com_ins.begin()+1; it != com_ins.end(); it++)
    {
        if(it->type == iFBEGIN) 
            isGlobal = 0;
        if(it->type == iFEND)
            isGlobal = 1;
        if(isGlobal == 0) //以所有内部语句作为起点
            q.push(&(*it));
    }
    while(!q.empty())
    {
        auto it = q.front();
        q.pop();
        if(it->type == iFEND) //边界特殊处理
        {
            for(auto pre_it : it-> preI)
                q.push(pre_it);
            continue;
        }
        bitset<1000> pre_live = it->live;
        //所有后继语句的并
        if(it->type == iGOTO)
            it->live = com_ins[label_table[it->arg1]].live;
        else if(it->type == iIF)
            it->live = com_ins[label_table[it->arg4]].live | com_ins[it->line_num+1].live;
        else it->live = com_ins[it->line_num+1].live;
        //注意，先去掉 def，再加上live，次序不能变
        it->live &= (~(it->def));
        it->live |= it->use;

        if(pre_live != it->live)
            for(auto pre_it : it-> preI)
                q.push(pre_it);
    }
    //生成每个变量的活跃区间
    int i;
    for(i=1;i<=Var_count;i++)
    {
        live_int[i].st = com_ins.size(), live_int[i].ed = -1, live_int[i].var = num_to_var[i];
        num_to_var[i]->LI = &live_int[i];
        live_int[i].spilled = 0;
    }
    for (auto it = com_ins.begin()+1; it != com_ins.end(); it++)
    {
        for(i=1;i<=Var_count;i++)
        {
            if(it->live[i]==1)
            {
                live_int[i].st = min(live_int[i].st, it->line_num-1); //需要 -1，因为这说明上一句是对它的一个赋值
                live_int[i].ed = max(live_int[i].ed, it->line_num);
            }
        }
    }
    for(i=1;i<=Var_count;i++)
        if(com_ins[live_int[i].st+1].type == iFBEGIN)//特殊情况，全局变量的活跃范围可能超出函数
            live_int[i].st+=2;
}

bool cmp_LI(const LiveInterval &a, const LiveInterval &b)
{
    if(a.st == b.st)
        return a.var < b.var;
    else
    {
        return a.st < b.st;
    }
}