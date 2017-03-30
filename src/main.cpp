#include "symbol.h"
#include "MiniC.tab.hpp"
int main()
{
    InitSymbolStack();
    yyparse();
    return 0;
}
