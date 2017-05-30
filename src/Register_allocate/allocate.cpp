#include "allocate.h"
#include <set>
#include <algorithm>
using namespace std;
set<Register*> free_reg, used_reg, all_reg;
map<Variable *, bool> hold_var;
extern LiveInterval live_int[1000];
extern int Var_count;
void init_all_reg()
{
    //保留 t0,t1 专门用作存放整数
    int i;
    for(i=2;i<=6;i++)
    {
        Register *now_reg = new Register();
        now_reg->var = nullptr;
        now_reg->r_name = "t"+to_string(i);
        all_reg.insert(now_reg);
    }
    for(i=2;i<=7;i++)
    {
        Register *now_reg = new Register();
        now_reg->var = nullptr;
        now_reg->r_name = "a"+to_string(i);
        all_reg.insert(now_reg);
    }
    for(i=0;i<=11;i++)
    {
        Register *now_reg = new Register();
        now_reg->var = nullptr;
        now_reg->r_name = "s"+to_string(i);
        all_reg.insert(now_reg);
    }
}

struct Comp_ptr_li
{
    bool operator()(LiveInterval* li1, LiveInterval* li2)
    {
        if(li1->ed == li2->ed)
            return li1 < li2;
        else
            return *(li1) < *(li2);
    }
};

set<LiveInterval*, Comp_ptr_li> act_li;
void ExpireOldInterval(int now_loc);
void SpillAnInterval(LiveInterval *now_li);
void LinearScan()
{
    int i;
    free_reg = all_reg;
    
    sort(live_int+1, live_int + Var_count + 1,cmp_LI);
    for (i = 1; i <= Var_count; i++)
    {
        ExpireOldInterval(live_int[i].st);
        if(free_reg.empty())
        {
            SpillAnInterval(&(live_int[i]));
        }
        else
        {
            live_int[i].reg = *free_reg.begin();
            used_reg.insert(*free_reg.begin());
            free_reg.erase(free_reg.begin());
            act_li.insert(&live_int[i]);
        }
    }
}

void ExpireOldInterval(int now_loc)
{
    while(!act_li.empty())
    {
        auto it = *(act_li.begin());
        if(it->ed < now_loc)
        {
            it->reg->free();
            used_reg.erase(it->reg);
            free_reg.insert(it->reg);
            act_li.erase(it);
        }
        else break;
    }
}

void SpillAnInterval(LiveInterval *now_li)
{
    auto it = act_li.end();
    it --;
    auto last_li = *it;
    if(now_li->ed < last_li->ed)
    {
        last_li->spilled = 1;
        now_li->reg = last_li->reg;
        act_li.erase(last_li);
        act_li.insert(now_li);
    }
    else
        now_li->spilled = 1;
}

extern vector<ins> com_ins;
void gen_output()
{
    bool isGlobal = 1;
    for (auto it = com_ins.begin()+1; it != com_ins.end(); it++)
    {
        if(it->type == iFBEGIN) 
            isGlobal = 0;
        if(it->type == iFEND)
            isGlobal = 1;
    }
}