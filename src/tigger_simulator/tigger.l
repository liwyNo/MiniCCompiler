WS  [ \t\v\n\f]
OP  "+"|"-"|"*"|"/"|"%"|">"|"<"|">="|"<="|"&&"|"||"|"!"|"!="|"=="
D   [0-9]
W   [a-zA-Z_0-9]

%{
#include <cstring>
#include <cstdio>
#include "tigger.tab.hpp"
#include "tigger.h"
void yyerror(const char *s);
%}

%%

("+"|"-"|{D}){D}*   {sscanf(yytext, "%d", &yylval.vint); return INT_CONSTANT;}
{OP}    {yylval.vstr = strdup(yytext); return OPERATOR;}
"["     {return '[';}
"]"     {return ']';}
"="     {return '=';}
":"     {return ':';}
f_{W}+  {yylval.vstr=strdup(yytext+2); return FUNCTION;}
l{D}+   {sscanf(yytext+1,"%d",&yylval.vint); return LABEL;}
x{D}+   {sscanf(yytext+1,"%d",&yylval.vint); return GVAR;}
call    {return CALL;}
if      {return IF;}
goto    {return GOTO;}
end     {return END;}
load    {return LOAD;}
store   {return STORE;}
malloc  {return MALLOC;}
s{D}+|a{D}+|t{D}+   {yylval.vint=find_reg(yytext); if (yylval.vint==-1) yyerror("register id not found"); return REGISTER;}
{WS}+   {}
.       {}

%%

int yywrap(void)
{
    return 1;
}

