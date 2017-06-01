#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "typedef.h"
#include <string>

struct Register
{
    std::string r_name;
    Variable *var;
    bool operator <(const Register &reg) const 
    {
        return r_name < reg.r_name;
    }
    void free();
};

struct LiveInterval //表示一个变量的活跃区间
{
    int st, ed;
    bool spilled; //是否被溢出
    Register *reg; //被分配的寄存器
    Variable *var;
    bool operator <(const LiveInterval &li) const
    {
        if(ed == li.ed)
            return var < li.var; //这个总不可能相同了
        return ed < li.ed;
    }
    void Print_Int();
};
bool cmp_LI(const LiveInterval &a, const LiveInterval &b);

void init_preI();
void LiveVariableAnalysis(ins *start_ins);
#endif