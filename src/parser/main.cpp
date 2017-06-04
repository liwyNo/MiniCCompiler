#include "symbol.h"
#include "gen.h"
#include "MiniC.tab.hpp"
#include <string>

void InitConstant()
{
    int c0 = CreateTempVar();
    int c1 = CreateTempVar();
    int i0 = 0;
    int i1 = 1;
    gen_const("int4", ('t' + std::to_string(c0)).c_str(), &i0);
    gen_const("int4", ('t' + std::to_string(c1)).c_str(), &i1);
}
int main(int argc, char *argv[])
{
    if (argc != 3) {
        puts("Usage: parser <input> <output>");
        exit(1);
    }
    freopen(argv[1], "r", stdin);
    freopen(argv[2], "w", stdout);
    InitSymbolStack();
    InitConstant();
    yyparse();
    return 0;
}
