#include "symbol.h"
#include "gen.h"
#include "MiniC.tab.hpp"
#include <string>

void InitConstant()
{
    int c0 = CreateConstant();
    int i0 = 0;
    gen_const("int4", ('c' + std::to_string(c0)).c_str(), &i0);
}
int main(int argc, char *argv[])
{
    InitSymbolStack();
    InitConstant();
    yyparse();
    return 0;
}
