#include "allocate.h"
#include <set>
#include <algorithm>
#include <string>
#include <iostream>
#include <stack>
#include <queue>
using namespace std;
#define debug(x) cerr<<#x<<"="<<x<<endl
set<Register*> free_reg, used_reg, all_reg;

extern LiveInterval live_int[1000];
extern int Var_count;
vector<string> callee_reg;
vector<string> caller_reg;
map<string, Register*> get_reg;
void init_all_reg()
{
    //保留 t0,t1 专门用作存放整数, t3专门存loadaddr上来的东西
    int i;
    for(i=3;i<=6;i++)
    {
        Register *now_reg = new Register();
        now_reg->var = nullptr;
        caller_reg.push_back("t"+to_string(i));
        now_reg->r_name = "t"+to_string(i);
        get_reg[now_reg->r_name] = now_reg;
        all_reg.insert(now_reg);
    }
    for(i=0;i<=11;i++)
    {
        Register *now_reg = new Register();
        now_reg->var = nullptr;
        now_reg->r_name = "s"+to_string(i);
        get_reg[now_reg->r_name] = now_reg;
        all_reg.insert(now_reg);
        callee_reg.push_back("s"+to_string(i));
    }
    for(i=0;i<=7;i++)
    {
        Register *now_reg = new Register();
        now_reg->var = nullptr;
        now_reg->r_name = "a"+to_string(i);
        get_reg[now_reg->r_name] = now_reg;
        all_reg.insert(now_reg);
        caller_reg.push_back("a"+to_string(i));
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
    used_reg.clear();
    act_li.clear();
    sort(live_int+1, live_int + Var_count + 1,cmp_LI); //排序之后地址会乱掉
    for (i = 1; i <= Var_count; i++)
        live_int[i].var->LI = &live_int[i];
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

int ins_num;//记录这是处理到全局的第几句了

void ExpireOldInterval(int now_loc)
{
    while(!act_li.empty())
    {
        auto it = *(act_li.begin());
        if(it->ed < now_loc)
        {
            it->reg->free();
            //it->var->reg = nullptr;
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

void __gen_Load_Int_Reg(int spill_loc, string r_name)
{
    cout << "load " << spill_loc << " " << r_name << endl;
}

void __gen_LDAD_Int_Reg(int spill_loc, string r_name)
{
    cout << "loadaddr " << spill_loc << " " << r_name << endl;
}


void __gen_ST_Reg_Int(string r_name, int spill_loc)
{
    cout << "store " << r_name << " " << spill_loc << endl;
}

void __gen_Reg_Int(int val, string r_name)
{
    cout << r_name << " = " << val << endl;
}

void __gen_LDAD_Gvar_Reg(string gvar, string r_name)
{
    cout << "loadaddr " << gvar << " " << r_name << endl;
}

void __gen_Load_Gvar_Reg(string v_name, string r_name)
{
    cout << "load " << v_name << " " << r_name << endl;
}

void __gen_Reg_Int_Ass_Reg(string r1, int idx, string r2)
{
    cout << r1 << " [" << idx << "]" << " = " << r2 << endl;
}

void __gen_Reg_Ass_Reg_Int(string r1, string r2, int idx)
{
    cout << r1 << " = " << r2 << " [" << idx << "]" << endl;
}

void __gen_R1_R2_OP2_R3(string r1, string r2, string r3, string op2)
{
    cout << r1 << " = " << r2 << " " << op2 << " " << r3 << endl;
}

void __gen_R1_OP1_R2(string r1, string r2, string op1)
{
    cout << r1 << " = " << " " << op1 << " " << r2 << endl;
}

void __gen_R1_Ass_R2(string r1, string r2)
{
    cout << r1 << " = " << r2 << endl;
}

void __gen_IF_R1_OP_R2_Goto(string r1, string rel_op, string r2, string lab)
{
    cout << "if " << r1 << " " << rel_op << " " << r2 << " goto " << lab << endl;
}

const string num_reg[2]={"t0","t1"};
const string addr_reg="t2";
int use_num_reg;
map<Variable *, bool> hold_var; //值为1表示这个变量在这个语句中出现了，值为2表示这个变量在这个语句中的寄存器分配过了
//因为可能会处理 t1 = t1 + t3 这种坑人的情况
inline void upd_hold(string v_name)
{
    if(isdigit(v_name[0]))
        return ;
    else 
        hold_var[get_Var(v_name)] = 1;
}

void LoadVar_Temp(Variable* lvar, string r_name) //仅函数传参和处理数组时用到，仅仅是Load 其他啥都不修改
{
    if(lvar->reg != nullptr)
    {
        __gen_R1_Ass_R2(r_name, lvar->reg->r_name);
        return;
    }
    if(lvar -> isArray == 1)
    {
        if(lvar-> isGlobal == 0)
            __gen_LDAD_Int_Reg(lvar->spill_loc, r_name);
        else
            __gen_LDAD_Gvar_Reg(lvar ->v_name, r_name);
    }
    else
    {
        if(lvar->isGlobal == 0)
            __gen_Load_Int_Reg(lvar->spill_loc, r_name);
        else
            __gen_Load_Gvar_Reg(lvar->v_name, r_name);
    }
}

void StoreVal(Variable *svar, Register* reg) //把 reg 内的值存到 svar 的内存里，只在取得函数返回值时使用
{
    if(svar->isGlobal == 0)
        __gen_ST_Reg_Int(reg->r_name, svar->spill_loc);
    else
    {
        __gen_LDAD_Gvar_Reg(svar->v_name, addr_reg);
        __gen_Reg_Int_Ass_Reg(addr_reg, 0, reg->r_name);
    }
}

void LoadVar(Variable* lvar, Register* reg)
{
    if(lvar -> isArray == 1)
    {
        if(lvar-> isGlobal == 0)
            __gen_LDAD_Int_Reg(lvar->spill_loc, reg->r_name);
        else
            __gen_LDAD_Gvar_Reg(lvar ->v_name, reg->r_name);
    }
    else
    {
        if(lvar-> isGlobal == 0)
            __gen_Load_Int_Reg(lvar->spill_loc, reg->r_name);
        else
            __gen_Load_Gvar_Reg(lvar->v_name, reg->r_name);
    }
    free_reg.erase(reg);
    used_reg.insert(reg);
    reg->var = lvar;
    lvar->reg = reg;
}

void SpillVar(Variable* svar)
{
    if(svar == nullptr)
    {
        debug("Warning nullptr occured in SpillVar function!");
        return;
    }
    if(svar->isArray)//数组没有实体，不能存回去
        ;
    else
    {
        if(svar->isGlobal == 0)
            __gen_ST_Reg_Int(svar->reg->r_name, svar->spill_loc);
        else
        {
            __gen_LDAD_Gvar_Reg(svar->v_name, addr_reg);
            __gen_Reg_Int_Ass_Reg(addr_reg, 0, svar->reg->r_name);
        }
    }
    used_reg.erase(svar->reg);
    free_reg.insert(svar->reg);
    svar->inMemory = 1;
    svar->reg->free(); //注意寄存器和变量之前是双向链接
    svar->reg = nullptr;
}

void Recover(Variable *svar) //恢复这个变量
{
    if(svar == nullptr)
        return;
    if(svar->LI->spilled == 0 && svar->LI->st <= ins_num && svar->LI->ed >= ins_num) //只有被钦定的变量，我们才去恢复它
    {
        if(svar->LI->reg->var != nullptr)
            SpillVar(svar->LI->reg->var);//把原先的赶走
        LoadVar(svar, svar->LI->reg);//新的加回来
    }
}

string get_Reg(string v_name, Variable* &be_spilled)
{
    be_spilled = nullptr;
    if(isdigit(v_name[0]))//假如是个数字，就给分配预留出来的两个寄存器之一
    {
        __gen_Reg_Int(stoi(v_name), num_reg[use_num_reg]);
        return num_reg[use_num_reg++];
    }
    auto my_var = get_Var(v_name);
    if(my_var->reg != nullptr)
        return my_var->reg->r_name;
    
    
    /*此时说明它在这个位置没有被Linear Scan钦定寄存器，需要找个空的，或者搞掉别人的寄存器*/
    if(!free_reg.empty())
    {
        LoadVar(my_var, *(free_reg.begin()));
        return my_var->reg->r_name;
    }
    else
    {
        for(auto spill_reg : used_reg)
        {
            if(hold_var[spill_reg->var]) //假如这句话要用这个变量，那么不能溢出它的寄存器
                continue;
            be_spilled = spill_reg->var;
            SpillVar(be_spilled);
            LoadVar(my_var, spill_reg);
            return my_var->reg->r_name;
        }
    }
} 

void ExpireOldInterval2(int now_loc);
extern vector<ins> com_ins;
extern vector<string> ori_ins;
void gen_output()
{
    bool isGlobal = 1;
    Function* now_fun = nullptr;
    int i, p_i = 1;
    int last_param=-1, arg_cnt; //cnt_arg 统计已经压进去多少个参数了
    stack<Variable*> sp_stk; //存储在调函数的过程中，被强制溢出的变量
    queue<Variable*> param_que;
    ins_num = 0;
    act_li.clear();

    for (auto it = com_ins.begin()+1; it != com_ins.end(); it++)
    {
        ins_num++;
        //debug(ins_num);
        if(it->type == iFBEGIN) 
            isGlobal = 0;
        if(it->type == iFEND)
            isGlobal = 1;
        /*钦定的寄存器在一开头就处理好*/
        if(isGlobal == 0 && it->type != iFBEGIN)
        {
            ExpireOldInterval2(ins_num);
            while(p_i <= Var_count && live_int[p_i].st <= ins_num)
            {
                if(!live_int[p_i].spilled)
                {
                    auto my_var = live_int[p_i].var;
                    auto reg = live_int[p_i].reg;
                    if(reg->var != nullptr)
                        SpillVar(reg->var);
                    LoadVar(my_var, reg);
                }
                p_i++;
            }
        }
        
        hold_var.clear();
        use_num_reg = 0;

        if(it->type == iOP2)
        {
            string R1, R2, R3;
            Variable *sp1, *sp2, *sp3;
            upd_hold(it->arg1), upd_hold(it->arg2), upd_hold(it->arg4);
            R1 = get_Reg(it->arg1, sp1);
            R2 = get_Reg(it->arg2, sp2);
            R3 = get_Reg(it->arg4, sp3);
            Variable *a = get_Var(it->arg1);
            //别忘修改第一个变量的inmemory
            a->inMemory = 0;
            __gen_R1_R2_OP2_R3(R1,R2,R3, it->arg3);
            Recover(sp1);
            Recover(sp2);
            Recover(sp3);
        }

        if(it->type == iOP1)
        {
            string R1, R2;
            Variable *sp1, *sp2;
            upd_hold(it->arg1), upd_hold(it->arg3);
            R1 = get_Reg(it->arg1, sp1);
            R2 = get_Reg(it->arg3, sp2);
            Variable *a = get_Var(it->arg1);
            //别忘修改第一个变量的inmemory
            a->inMemory = 0;
            __gen_R1_OP1_R2(R1, R2, it->arg2);
            Recover(sp1);
            Recover(sp2);
        }

        if(it->type == iASS)
        {
            if(isGlobal)
                continue;
            string R1, R2;
            Variable *sp1, *sp2;
            upd_hold(it->arg1), upd_hold(it->arg2);
            R1 = get_Reg(it->arg1, sp1);
            R2 = get_Reg(it->arg2, sp2);
            Variable *a = get_Var(it->arg1);
            //别忘修改第一个变量的inmemory
            a->inMemory = 0;
            __gen_R1_Ass_R2(R1, R2);
            Recover(sp1);
            Recover(sp2);
        }

        if(it->type == iARRSET)
        {
            string R1, R2, R3;
            Variable *sp1, *sp2, *sp3;
            upd_hold(it->arg1), upd_hold(it->arg2), upd_hold(it->arg3);
            R2 = get_Reg(it->arg2, sp2);
            R3 = get_Reg(it->arg3, sp3);
            Variable *a = get_Var(it->arg1);
            if(a->isArray == 1)
            {
                if(a->isGlobal == 0)
                    __gen_LDAD_Int_Reg(a->spill_loc, addr_reg);
                else
                    __gen_LDAD_Gvar_Reg(a->v_name, addr_reg);
            }
            else LoadVar_Temp(a, addr_reg);
            __gen_R1_R2_OP2_R3(addr_reg, addr_reg, R2, "+");
            __gen_Reg_Int_Ass_Reg(addr_reg, 0, R3);
            Recover(sp2);
            Recover(sp3);
        }
        if(it->type == iARRGET)
        {
            string R1, R2, R3;
            Variable *sp1, *sp2, *sp3, *trash;
            upd_hold(it->arg1), upd_hold(it->arg2), upd_hold(it->arg3);
            R1 = get_Reg(it->arg1, sp1);
            R3 = get_Reg(it->arg3, sp3);
            Variable *a = get_Var(it->arg1), *b = get_Var(it->arg2);
            if(b->isArray == 1)
            {
                if(b->isGlobal == 0)
                    __gen_LDAD_Int_Reg(b->spill_loc, addr_reg);
                else
                    __gen_LDAD_Gvar_Reg(b->v_name, addr_reg);
            }
            else LoadVar_Temp(b, addr_reg);
            __gen_R1_R2_OP2_R3(addr_reg, addr_reg, R3, "+");
            __gen_Reg_Ass_Reg_Int(R1, addr_reg, 0);
            a->inMemory = 0;
            Recover(sp1);
            Recover(sp3);
        }

        if(it->type == iIF)
        {
            string R1, R2;
            Variable *sp1, *sp2;
            upd_hold(it->arg1), upd_hold(it->arg3);
            R1 = get_Reg(it->arg1, sp1);
            R2 = get_Reg(it->arg3, sp2);
            __gen_IF_R1_OP_R2_Goto(R1, it->arg2, R2, it->arg4);
            Recover(sp1);
            Recover(sp2);
        }

        if(it->type == iGOTO) // goto 语句没有变化，直接输出
        {
            cout << ori_ins[it - com_ins.begin()] << endl;
        }

        if(it->type == iPARAM)
        {
            //假装一切没有发生过，只把参数名字默默的记下来
            Variable *a = get_Var(it->arg1);
            param_que.push(a);
            /*
            if(last_param == ins_num - 1)
                arg_cnt ++;
            else
            {
                arg_cnt = 0;
                //把所有 caller save 都存起来
                for(auto reg_name: caller_reg)
                {
                    auto reg_it = get_reg[reg_name];
                    if(reg_it->var != nullptr)
                    {
                        sp_stk.push(reg_it->var);
                        SpillVar(reg_it->var);
                    }
                }
            }
            last_param = ins_num;
            string R1;
            Variable *sp1;
            //upd_hold(it->arg1);
            Register* pas_reg = get_reg["a"+to_string(arg_cnt)];
            
            Variable *a = get_Var(it->arg1);
            LoadVar_Temp(a, pas_reg);
            */
        }

        if(it->type == iCALLVOID)
        {
            for(auto reg_name: caller_reg)
            {
                auto reg_it = get_reg[reg_name];
                if(reg_it->var != nullptr)
                {
                    sp_stk.push(reg_it->var);
                    SpillVar(reg_it->var);
                }
            }
            int param_cnt=0;
            while(!param_que.empty())
            {
                Register* pas_reg = get_reg["a"+to_string(param_cnt)];
                auto param_var = param_que.front();
                param_que.pop();
                if(param_var->reg != nullptr) //为了省事，都先放到内存里
                {
                    sp_stk.push(param_var);
                    SpillVar(param_var);
                }
                LoadVar_Temp(param_var, pas_reg->r_name);
                param_cnt++;
            }
            
            cout << ori_ins[ins_num] << endl;
            while(!sp_stk.empty())
            {
                Recover(sp_stk.top()); //为了省事，也只恢复被钦定的，其他的就不管了
                sp_stk.pop();
            }
        }

        if(it->type == iCALL)
        {
            
            //把所有 caller save 都存起来
            for(auto reg_name: caller_reg)
            {
                auto reg_it = get_reg[reg_name];
                if(reg_it->var != nullptr)
                {
                    sp_stk.push(reg_it->var);
                    SpillVar(reg_it->var);
                }
            }
            //debug("save caller");
            int param_cnt=0;
            while(!param_que.empty())
            {
                Register* pas_reg = get_reg["a"+to_string(param_cnt)];
                auto param_var = param_que.front();
                //debug((param_var == nullptr));
                //param_var->Print_Var();
                param_que.pop();
                if(param_var->reg != nullptr) //为了省事，都先放到内存里
                {
                    sp_stk.push(param_var);
                    SpillVar(param_var);
                }
                LoadVar_Temp(param_var, pas_reg->r_name);
                param_cnt++;
            }

            //cout << ori_ins[ins_num] << endl;
            cout << "call " << it->arg2 << endl;
            Variable *a = get_Var(it->arg1);
            if(a->reg != nullptr)
            {
                sp_stk.push(a);
                SpillVar(a);//就算之前有，也给搞出来
            }
            StoreVal(a, get_reg["a0"]);
            while(!sp_stk.empty())
            {
                Recover(sp_stk.top()); //为了省事，也只恢复被钦定的，其他的就不管了
                sp_stk.pop();
            }
        }

        if(it->type == iRETURN)
        {
            string R1;
            Variable *sp1;
            R1 = get_Reg(it->arg1,sp1);
            __gen_R1_Ass_R2("a0", R1);
            //到末尾了，也不用管恢复溢出变量了

            //恢复 callee save
            for(i = 0; i < callee_reg.size(); i++)
            {
                string reg_str = callee_reg[i];
                __gen_Load_Int_Reg(i, reg_str);
            }
            cout << "return" << endl;
        }

        if(it->type == iVAR || it->type == iNOOP) //局部变量不用声明了，直接就没了。。。不用管
            continue;
        if(it->type == iGVAR)
        {
            Variable* a= get_Var_in_Func(it->arg1, nullptr);
            if(a->isArray == 0)
            {
                string init_num = "0";
                if(it+1!=com_ins.end()) //Eeyore 可能在下一句有赋值
                    if((it+1)->type == iASS)
                        init_num = (it+1)->arg2;
                cout << a->v_name << " = " << init_num << endl;
            }
            else
            {
                string init_num = it->arg2;
                cout << a->v_name << " = malloc " << init_num << endl;
            }
        }
        if(it->type == iLABEL)
        {
            //cout << it->arg1 << endl;
            //cout << it->arg1 <<":"<<endl;
            cout << ori_ins[it - com_ins.begin()] << endl;
        }
        
        if(it->type == iFBEGIN)//iFBEGIN 后面要加一个中括号
        {
            for (auto reg: all_reg)
                reg->free();
            free_reg = all_reg;
            used_reg.clear();

            LiveVariableAnalysis(&(*it));
            LinearScan();
            //init_debug();
            
            for (auto reg: all_reg)
                reg->free();
            free_reg = all_reg;
            used_reg.clear();
            act_li.clear();
            p_i = 1;
            
            cout << ori_ins[it - com_ins.begin()];
            now_fun = get_Fun(it->arg1);
            cout << " [" << now_fun->stack_size << "]" << endl;

            //存储 callee save
            for(i = 0; i < callee_reg.size(); i++)
            {
                string reg_str = callee_reg[i];
                __gen_ST_Reg_Int(reg_str, i);
            }
            //debug("end store reg");
            for (i = 0; i < now_fun->arg_num; i++)//把函数参数全都放到内存中再说，有点牺牲效率，但否则会各种特判
            {
                auto now_arg = get_Var_in_Func(extend_p_name("p"+to_string(i), it->arg1), now_fun);
                __gen_ST_Reg_Int("a"+to_string(i), now_arg->spill_loc);
                now_arg->inMemory = 1;
            }
            //debug("end Fbegin");
        }
        if(it->type == iFEND)
        {
            //恢复 callee save
            
            cout << ori_ins[it - com_ins.begin()] << endl;
            now_fun = nullptr;
        }
    }
}

void ExpireOldInterval2(int now_loc)
{
    while(!act_li.empty())
    {
        auto it = *(act_li.begin());
        if(it->ed < now_loc)
        {
            it->reg->free();
            SpillVar(it->var); //区别在于多了输出变量的溢出
            used_reg.erase(it->reg);
            free_reg.insert(it->reg);
            act_li.erase(it);
        }
        else break;
    }
}

/*
#define iOP2	1
#define iOP1	2
#define iASS	3
#define iARRSET	4
#define iARRGET	5
#define iIF	6
#define iGOTO	7
#define iPARAM	8
#define iCALLVOID	9
#define iCALL 	10
#define iRETURN	11
#define iVAR	12
#define iGVAR 	13
#define iFBEGIN 14
#define iFEND	15
#define iLABEL  16
#define iNOOP	0
*/