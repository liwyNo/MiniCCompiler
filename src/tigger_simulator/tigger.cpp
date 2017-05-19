#include "yaccTypes.h"

std::vector<int> stackmem;
int reg[REGNUM];
std::vector<int> gvars;
std::vector<statement_t *> stmts;
std::map<int, int> labels;
std::map<std::string, int> funcs;
std::map<int, int> gvar_name;
int ngvar;
int pc, sp, ssp;
std::stack<CallStack_t> callstack;
const char *str_reg[REGNUM]={"s0","s1","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","a0","a1","a2","a3","a4","a5","a6","a7","t0","t1","t2","t3","t4","t5","t6"};

int find_reg(const char *s)
{
    for (int i = 0; i < REGNUM; ++i)
        if (strcmp(s, str_reg[i]) == 0)
            return i;
    return -1;
}

int main(int argc, char *argv[])
{
    yyparse();

    if (funcs.find("main") == funcs.end())
        yyerror("main function not found");

    pc = funcs["main"];
    sp = 0;
    while (1)
        stmts[pc]->run();
    return 0;
}