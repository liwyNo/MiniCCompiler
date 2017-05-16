%{
#include <cstdio>
int yylex(void);
%}

%code requires {
void yyerror(const char *s);
}

%union {
    char *vstr;
    int vint;
}

%token <vint> INT_CONSTANT TEMPVAR ORIGVAR LABEL PARAVAR
%token <vstr> OPERATOR FUNCTION
%token VAR END PARAM IF GOTO CALL RETURN

%start translation_unit

%%

translation_unit: statement_function_list
                ;

statement_list: statement
              |
              | statement_list statement
              ;

statement_function_list: statement
                       | function
                       |
                       | statement_function_list statement
                       | statement_function_list function
                       ;

statement: variable '=' variable_constant OPERATOR variable_constant
         | variable '=' variable_constant
         | variable '=' OPERATOR variable_constant
         | variable '=' variable '[' variable_constant ']'
         | variable '[' variable_constant ']' '=' variable_constant
         | OPERATOR variable '=' variable_constant
         | GOTO LABEL
         | IF variable_constant OPERATOR variable_constant GOTO LABEL
         | LABEL ':'
         | variable '=' CALL FUNCTION INT_CONSTANT
         | RETURN variable_constant
         | PARAM variable_constant
         | variable_declare
         ;

variable_constant: variable
                 | INT_CONSTANT

function: FUNCTION ':' statement_list END FUNCTION
        ;

variable_declare: VAR variable
                /* | GVAR variable */
                | VAR INT_CONSTANT variable
                /* | GVAR INT_CONSTANT variable */
                ;

variable: PARAVAR
        | TEMPVAR
        | ORIGVAR
        ;

%%
void yyerror(const char *s)
{
    puts(s);
    exit(1);
}
