#include "symbol.h"
#include "gen.h"
#include "MiniC.tab.hpp"
int main(int argc, char *argv[])
{
    if (argc <= 1)
        return 1;
    InitSymbolStack();
    set_output(argv[1]);
    yyparse();
    return 0;
}
