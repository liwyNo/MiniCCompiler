%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "yaccTypes.h"
int yylex(void);
%}

%code requires {
void yyerror(const char *s);
#define REGNUM 27
extern const char *str_reg[REGNUM];
}

%union {
    char *vstr;
    int vint;
}

%token <vint> INT_CONSTANT GVAR LABEL REGISTER
%token <vstr> OPERATOR FUNCTION
%token END IF GOTO CALL LOAD STORE MALLOC

%%

start: block
     |
     ;

block: function_declare
     | global_var_declare
     | block function_declare
     | block global_var_declare
     ;

function_declare: FUNCTION '[' INT_CONSTANT ']' '[' INT_CONSTANT ']' ':' {
                        add_func_begin($1, $3, $6);
                    } expression_list END FUNCTION {
                        if (strcmp($1, $12) != 0)
                            yyerror("function declare name not match");
                        stmts.push_back(new stmt_func_end($1));
                    }
                ;

global_var_declare: GVAR '=' MALLOC INT_CONSTANT { add_gvar_array($1, $4); }
                  | GVAR '=' INT_CONSTANT   { add_gvar_int($1, $3); }
                  ;

expression_list: 
               | expression_list expression
               ;

expression: REGISTER '=' INT_CONSTANT   {stmts.push_back(new stmt_assign_const($1, $3));}
          | REGISTER '=' REGISTER OPERATOR REGISTER     {if (!check_op2($4)) yyerror("OP2 required"); stmts.push_back(new stmt_assign_op2($1, $3, $4, $5));}
          | REGISTER '=' OPERATOR REGISTER  {if (!check_op1($3)) yyerror("OP1 required"); stmts.push_back(new stmt_assign_op1($1, $3, $4));}
          | REGISTER '=' REGISTER   {stmts.push_back(new stmt_assign_move($1, $3));}
          | REGISTER '[' INT_CONSTANT ']' '=' REGISTER  {stmts.push_back(new stmt_assign_lidx($1, $3, $6));}
          | REGISTER '=' REGISTER '[' INT_CONSTANT ']'  {stmts.push_back(new stmt_assign_ridx($1, $3, $5));}
          | IF REGISTER OPERATOR REGISTER GOTO LABEL    {if (!check_op2($3)) yyerror("OP2 required"); stmts.push_back(new stmt_if_goto($2, $3, $4, $6));}
          | GOTO LABEL  {stmts.push_back(new stmt_goto($2));}
          | LABEL ':'   {add_label($1);}
          | CALL FUNCTION   {stmts.push_back(new stmt_call($2));}
          | STORE REGISTER INT_CONSTANT {stmts.push_back(new stmt_store_local($2, $3));}
          | STORE REGISTER GVAR {if (gvar_name.find($3) == gvar_name.end()) yyerror("gvar not declared"); stmts.push_back(new stmt_store_global($2, gvar_name[$3]));}
          | LOAD INT_CONSTANT REGISTER  {stmts.push_back(new stmt_load_local($2, $3));}
          | LOAD GVAR REGISTER  {if (gvar_name.find($3) == gvar_name.end()) yyerror("gvar not declared"); stmts.push_back(new stmt_load_global($2, gvar_name[$3]));}
          | REGISTER '=' MALLOC INT_CONSTANT {if ($4 % 4 != 0 || $4 == 0) yyerror("what do you mean by this mallocing size?"); stmts.push_back(new stmt_malloc($1, $4));}
          ;

%%

void yyerror(const char *s)
{
    puts(s);
    exit(1);
}
