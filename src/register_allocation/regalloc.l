WS  [ \t\v\n\f]
OP  "+"|"-"|"*"|"/"|"%"|">"|"<"|">="|"<="|"&&"|"||"|"!"|"!="|"=="
D   [0-9]
W   [a-zA-Z_0-9]

%{
#include <cstring>
#include <cstdio>
#include "regalloc.tab.hpp"
void yyerror(const char *s);
%}

%%

"var"   {return VAR;}
"gvar"  {return GVAR;}
{OP}    {yylval.vstr = strdup(yytext); return OPERATOR;}
("+"|"-"|{D}){D}*   {sscanf(yytext, "%d", &yylval.vint); return INT_CONSTANT;}
"["     {return '[';}
"]"     {return ']';}
"="     {return '=';}
":"     {return ':';}
t{D}+   {sscanf(yytext+1,"%d",&yylval.vint); return TEMPVAR;}
T{D}+   {sscanf(yytext+1,"%d",&yylval.vint); return ORIGVAR;}
l{D}+   {sscanf(yytext+1,"%d",&yylval.vint); return LABEL;}
p{D}+   {sscanf(yytext+1,"%d",&yylval.vint); return PARAVAR;}
f_{W}+  {yylval.vstr=strdup(yytext+2); return FUNCTION;}
end     {return END;}
param   {return PARAM;}
call    {return CALL;}
return  {return RETURN;}
if      {return IF;}
goto    {return GOTO;}
{WS}+   {}
.       {}

%%

int yywrap(void)
{
    return 1;
}
