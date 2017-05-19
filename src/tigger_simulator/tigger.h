#ifndef TIGGER_H
#define TIGGER_H

#include <vector>
#include <map>
#include <string>
#include <stack>
#include "tigger.tab.hpp"

class statement_t;

extern std::vector<int> stackmem;
extern int reg[REGNUM];
extern std::vector<int> gvars;
extern std::vector<statement_t *> stmts;
extern std::map<int, int> labels;
extern std::map<std::string, int> funcs;
extern std::map<int, int> gvar_name;
extern int ngvar;
extern int pc, sp, ssp;
struct CallStack_t {
    int pc; /* program counter */
    int sp; /* stack pointer */
    int ssp; /* total stack space to use */
    CallStack_t(int _pc, int _sp, int _ssp): pc(_pc), sp(_sp), ssp(_ssp) {}
};
extern std::stack<CallStack_t> callstack;
extern const char *str_reg[REGNUM];
int find_reg(const char *s);

#endif
