WS  [ \t\v\n\f]
OP  "+"|"-"|"*"|"/"|"%"|">"|"<"|">="|"<="|"&&"|"||"|"!"|"!="|"=="
D   [0-9]
W   [a-zA-Z_0-9]

%{
#include <cstring>
#include <cstdio>
#include "riscv.tab.hpp"
void yyerror(const char *s);
%}

%option yylineno

%%

"+"     {return '+';}
"-"     {return '-';}
"*"     {return '*';}
"/"     {return '/';}
"%"     {return '%';}
">"     {return '>';}
"<"     {return '<';}
">="     {return GE;}
"<="     {return LE;}
"&&"     {return AND;}
"||"     {return OR;}
"!"     {return '!';}
"=="     {return EQ;}
"!="     {return NE;}
{D}+   {sscanf(yytext, "%d", &yylval.vint); return INT_CONSTANT;}
"["     {return '[';}
"]"     {return ']';}
"="     {return '=';}
":"     {return ':';}
f_{W}+  {yylval.vstr=strdup(yytext+2); return FUNCTION;}
l{D}+   {yylval.vstr=strdup(yytext); return LABEL;}
v{D}+   {yylval.vstr=strdup(yytext); return GVAR;}
call    {return CALL;}
if      {return IF;}
goto    {return GOTO;}
end     {return END;}
load    {return LOAD;}
store   {return STORE;}
loadaddr {return LOADADDR;}
malloc  {return MALLOC;}
x0  {yylval.vstr=strdup("zero"); return REGISTER;}
s([0-9]|10|11)  {yylval.vstr=strdup(yytext); return REGISTER;}
a[0-7]  {yylval.vstr=strdup(yytext); return REGISTER;}
t[0-6]  {yylval.vstr=strdup(yytext); return REGISTER;}
{WS}+   {}
.       {}

%%

int yywrap(void)
{
    return 1;
}

