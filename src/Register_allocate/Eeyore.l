NUM -?[1-9][0-9]*|0
SYMBOL [Ttp][0-9]+
LABEL l[0-9]+
FUNCTION f_[_a-zA-Z][_a-zA-Z0-9]*
LOGIOP !=|==|>|<
OP [\+\-\*\/\%\!]|\|\||&&

%{
#include "stdio.h"
void yyerror(const char*);
#include "Eeyore.tab.hpp"
#define printf(x) 
%}

%option yylineno

%%
" " 
"\t"
\/\/.*
"\n"	{printf("Lex: EOL\n"); return EOL;}

"var"	{printf("Lex: VAR\n"); return VAR;}
"if"	{printf("Lex: IF\n"); return IF;}
"goto"	{printf("Lex: GOTO\n"); return GOTO;}
"param"	{printf("Lex: PARAM\n"); return PARAM;}
"call"	{printf("Lex: CALL\n"); return CALL;}
"return" {printf("Lex: RETURN\n"); return RETURN;}
"end"	{printf("Lex: END\n"); return ENDT;}


{LABEL}	{printf("Lex: LABEL\n"); yylval.vstr = strdup(yytext); return LABEL;}
{NUM}	{printf("Lex: NUM\n"); yylval.vstr = strdup(yytext); return NUM;}
{SYMBOL} {printf("Lex: SYMBOL\n"); yylval.vstr = strdup(yytext); return SYMBOL;}
{FUNCTION}	{printf("Lex: FUNCTION\n"); yylval.vstr = strdup(yytext);return FUNCTION;}
[\[\]:=] {printf("Lex: \\\\=\n"); return *strdup(yytext);}
{OP}	{printf("Lex: OP\n"); yylval.vstr = strdup(yytext); return OP;}
{LOGIOP} {printf("Lex: LOGIOP\n"); yylval.vstr = strdup(yytext);return LOGIOP;}
%%
int yywrap(){
	return 1;
}
