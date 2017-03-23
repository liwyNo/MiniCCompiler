O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]

%s INSTRING

%{
#include <cstdio>
#include <cstring>
#include <string>
#include "MiniC.tab.hpp"
#include "symbol.h"

extern void yyerror(const char *);  /* prints grammar violation message */

std::string strbuf;

void comment(void);
int check_type(void);

%}

%%

<INSTRING>\\a   {strbuf += '\a';}
<INSTRING>\\b   {strbuf += '\b';}
<INSTRING>\\f   {strbuf += '\f';}
<INSTRING>\\n   {strbuf += '\n';}
<INSTRING>\\t   {strbuf += '\t';}
<INSTRING>\\v   {strbuf += '\v';}
<INSTRING>\\\\   {strbuf += '\\';}
<INSTRING>\\'   {strbuf += '\'';}
<INSTRING>\\\"   {strbuf += '\"';}
<INSTRING>\\?   {strbuf += '\?';}
<INSTRING>\\{O}+   {char c; sscanf(yytext, "\\%hho", &c); strbuf += c;}
<INSTRING>\\x{H}+   {char c; sscanf(yytext, "\\x%hhx", &c); strbuf += c;}
<INSTRING>[^\\\n"]*   {strbuf += yytext;}
<INSTRING>\"{WS}*\" {}
<INSTRING>\"    {yylval.vstr = strdup(strbuf.c_str()); BEGIN 0; return STR_CONSTANT;}

"/*"                                    { comment(); }
"//".*                                    { /* consume //-comment */ }

"break"					{ return(BREAK); }
"case"					{ return(CASE); }
"char"					{ return(CHAR); }
"const"					{ return(CONST); }
"continue"				{ return(CONTINUE); }
"default"				{ return(DEFAULT); }
"do"					{ return(DO); }
"double"				{ return(DOUBLE); }
"else"					{ return(ELSE); }
"enum"					{ return(ENUM); }
"float"					{ return(FLOAT); }
"for"					{ return(FOR); }
"goto"					{ return(GOTO); }
"if"					{ return(IF); }
"int"					{ return(INT); }
"long"					{ return(LONG); }
"return"				{ return(RETURN); }
"short"					{ return(SHORT); }
"signed"				{ return(SIGNED); }
"sizeof"				{ return(SIZEOF); }
"static"				{ return(STATIC); }
"struct"				{ return(STRUCT); }
"switch"				{ return(SWITCH); }
"typedef"				{ return(TYPEDEF); }
"union"					{ return(UNION); }
"unsigned"				{ return(UNSIGNED); }
"void"					{ return(VOID); }
"while"					{ return(WHILE); }

{L}{A}*					{ return check_type(); }

{HP}{H}+{IS}?				{ sscanf(yytext, "%x", &yylval.vint); return INT_CONSTANT; }
{NZ}{D}*{IS}?				{ sscanf(yytext, "%o", &yylval.vint); return INT_CONSTANT; }
"0"{O}*{IS}?				{ sscanf(yytext, "%d", &yylval.vint); return INT_CONSTANT; }
'\\a'		{ yylval.vchar = '\a'; return CHAR_CONSTANT; }
'\\b'		{ yylval.vchar = '\b'; return CHAR_CONSTANT; }
'\\f'		{ yylval.vchar = '\f'; return CHAR_CONSTANT; }
'\\n'		{ yylval.vchar = '\n'; return CHAR_CONSTANT; }
'\\t'		{ yylval.vchar = '\t'; return CHAR_CONSTANT; }
'\\v'		{ yylval.vchar = '\v'; return CHAR_CONSTANT; }
'\\\\'	    { yylval.vchar = '\\'; return CHAR_CONSTANT; }
'\\''		{ yylval.vchar = '\''; return CHAR_CONSTANT; }
'\\\"'		{ yylval.vchar = '\"'; return CHAR_CONSTANT; }
'\\?'		{ yylval.vchar = '\?'; return CHAR_CONSTANT; }
'\\{O}+'    { sscanf(yytext, "'\\%hho'", &yylval.vchar); return CHAR_CONSTANT; }
'\\x{H}+'    { sscanf(yytext, "'\\x%hhx'", &yylval.vchar); return CHAR_CONSTANT; }
'[^\\\n']'   { yylval.vchar = yytext[1]; return CHAR_CONSTANT; }

{D}+{E}{FS}?				{ sscanf(yytext, "%lf", &yylval.vdouble); return DOUBLE_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ sscanf(yytext, "%lf", &yylval.vdouble); return DOUBLE_CONSTANT; }
{D}+"."{E}?{FS}?			{ sscanf(yytext, "%lf", &yylval.vdouble); return DOUBLE_CONSTANT; }
{HP}{H}+{P}{FS}?			{ sscanf(yytext, "%lf", &yylval.vdouble); return DOUBLE_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ sscanf(yytext, "%lf", &yylval.vdouble); return DOUBLE_CONSTANT; }
{HP}{H}+"."{P}{FS}?			{ sscanf(yytext, "%lf", &yylval.vdouble); return DOUBLE_CONSTANT; }

\"              { BEGIN INSTRING; strbuf = ""; }

">>="					{ return RIGHT_ASSIGN; }
"<<="					{ return LEFT_ASSIGN; }
"+="					{ return ADD_ASSIGN; }
"-="					{ return SUB_ASSIGN; }
"*="					{ return MUL_ASSIGN; }
"/="					{ return DIV_ASSIGN; }
"%="					{ return MOD_ASSIGN; }
"&="					{ return AND_ASSIGN; }
"^="					{ return XOR_ASSIGN; }
"|="					{ return OR_ASSIGN; }
">>"					{ return RIGHT_OP; }
"<<"					{ return LEFT_OP; }
"++"					{ return INC_OP; }
"--"					{ return DEC_OP; }
"->"					{ return PTR_OP; }
"&&"					{ return AND_OP; }
"||"					{ return OR_OP; }
"<="					{ return LE_OP; }
">="					{ return GE_OP; }
"=="					{ return EQ_OP; }
"!="					{ return NE_OP; }
";"					    { return ';'; }
"{"				    { return '{'; }
"}"			    	{ return '}'; }
","					{ return ','; }
":"					{ return ':'; }
"="					{ return '='; }
"("					{ return '('; }
")"					{ return ')'; }
"["	    			{ return '['; }
"]"		    		{ return ']'; }
"."					{ return '.'; }
"&"					{ return '&'; }
"!"					{ return '!'; }
"~"					{ return '~'; }
"-"					{ return '-'; }
"+"					{ return '+'; }
"*"					{ return '*'; }
"/"					{ return '/'; }
"%"					{ return '%'; }
"<"					{ return '<'; }
">"					{ return '>'; }
"^"					{ return '^'; }
"|"					{ return '|'; }
"?"					{ return '?'; }

{WS}+					{ /* whitespace separates tokens */ }
.					{ /* discard bad characters */ }

%%

int yywrap(void)        /* called at end of input */
{
    return 1;           /* terminate now */
}

void comment(void)
{
    int c;

    while ((c = yyinput()) != 0)
        if (c == '*')
        {
            while ((c = yyinput()) == '*')
                ;

            if (c == '/')
                return;

            if (c == 0)
                break;
        }
    yyerror("unterminated comment");
}

int check_type(void)
{
    int type;
    void *p = LookupSymbol(yytext, &type);
    switch (type)
    {
    case TYPE_NAME:
        yylval.vptr = p;
        return TYPE_NAME;
    case ENUM_CONSTANT:
        yylval.vint = *(int*)p;
        return ENUM_CONSTANT;
    default:
        yylval.vstr = strdup(yytext);
        return IDENTIFIER;
    }
}
