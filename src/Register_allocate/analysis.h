#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "typedef.h"

struct LiveInterval //表示一个变量的活跃区间
{
    int st, ed;
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
void LiveVariableAnalysis();
#endif