%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
int yylex(void);
extern int yylineno;
%}

%code requires {
void yyerror(const char *s);
#define REGNUM 28
extern const char *str_reg[REGNUM];
}

%union {
    char *vstr;
    int vint;
}

%token <vint> INT_CONSTANT 
%token <vstr> FUNCTION REGISTER LABEL GVAR
%token END IF GOTO CALL LOAD STORE MALLOC LOADADDR
%token GE LE AND OR NE EQ

%type <vint> integer
%type <vstr> operator

%%

start: block
     |
     ;

block: function_declare
     | global_var_declare
     | block function_declare
     | block global_var_declare
     ;

function_declare: FUNCTION '[' INT_CONSTANT ']' '[' INT_CONSTANT ']' {
                        int stk_s = ($6 / 4 + 1) * 16;
                        printf("\t.text\n");
                        printf("\t.align\t2\n");
                        printf("\t.global\t%s\n", $1);
                        printf("\t.type\t%s, @function\n", $1);
                        printf("%s:\n", $1);
                        printf("\tadd\tsp,sp,-%d\n", stk_s);
                        printf("\tsw\tra,%d(sp)\n", stk_s - 4);
                    } expression_list END FUNCTION {
                        int stk_s = ($6 / 4 + 1) * 16;
                        printf("\tlw\tra,%d(sp)\n", stk_s - 4);
                        printf("\tadd\tsp,sp,%d\n", stk_s);
                        printf("\tjr\tra\n");
                        printf("\t.size\t%s, .-%s\n", $1, $1);
                    }
                ;

integer: INT_CONSTANT   {$$=$1;}
       | '-' INT_CONSTANT {$$=-$2;}

global_var_declare: GVAR '=' MALLOC INT_CONSTANT {
                        printf("\t.comm\t%s,%d,4\n", $1, $4 * 4);
                  }
                  | GVAR '=' integer   {
                        printf("\t.global\t%s\n", $1);
                        printf("\t.section\t.sdata\n");
                        printf("\t.align 2\n");
                        printf("\t.type\t%s, @object\n", $1);
                        printf("\t.size\t%s, 4\n", $1);
                        printf("%s:\n", $1);
                        printf("\t.word\t%d\n", $3);
                  }
                  ;

expression_list: 
               | expression_list expression
               ;

operator: '+' {$$=strdup("+");}
        | '-' {$$=strdup("-");}
        | '*' {$$=strdup("*");}
        | '/' {$$=strdup("/");}
        | '%' {$$=strdup("%");}
        | '>' {$$=strdup(">");}
        | '<' {$$=strdup("<");}
        | LE {$$=strdup("@");}
        | GE {$$=strdup("#");}
        | AND {$$=strdup("&&");}
        | OR {$$=strdup("||");}
        | NE {$$=strdup("!=");}
        | EQ {$$=strdup("==");}
        ;

expression: REGISTER '=' integer   {printf("\tli\t%s,%d\n", $1, $3);}
          | REGISTER '=' REGISTER operator REGISTER     {
                switch($4[0]){
                    case '+':
                        printf("\tadd\t%s,%s,%s\n", $1, $3, $5);
                        break;
                    case '-':
                        printf("\tsub\t%s,%s,%s\n", $1, $3, $5);
                        break;
                    case '*':
                        printf("\tmul\t%s,%s,%s\n", $1, $3, $5);
                        break;
                    case '/':
                        printf("\tdiv\t%s,%s,%s\n", $1, $3, $5);
                        break;
                    case '%':
                        printf("\trem\t%s,%s,%s\n", $1, $3, $5);
                        break;
                    case '<':
                        printf("\tslt\t%s,%s,%s\n", $1, $3, $5);
                        break;
                    case '@': //<=
                        printf("\tsub\t%s,%s,%s\n", $1, $3, $5);
                        printf("\tsgtz\t%s,%s\n", $1, $1);
                        printf("\txori\t%s,%s,1\n", $1, $1);
                        break;
                    case '#': //>=
                        printf("\tsub\t%s,%s,%s\n", $1, $3, $5);
                        printf("\tsltz\t%s,%s\n", $1, $1);
                        printf("\txori\t%s,%s,1\n", $1, $1);
                        break;
                    case '>':
                        printf("\tsgt\t%s,%s,%s\n", $1, $5, $3);
                        break;
                    case '&':
                        //Need better solution
                        printf("\tseqz\t%s,%s\n", $1, $3);
                        printf("\tadd\t%s,%s,-1\n", $1, $1);
                        printf("\tand\t%s,%s,%s\n", $1, $1, $5);
                        printf("\tsnez\t%s,%s\n", $1, $1);
                        break;
                    case '|':
                        printf("\tor\t%s,%s,%s\n", $1, $3, $5);
                        printf("\tsnez\t%s,%s\n", $1, $1);
                        break;
                    case '!':
                        printf("\txor\t%s,%s,%s\n", $1, $3, $5);
                        printf("\tsnez\t%s,%s\n", $1, $1);
                        break;
                    case '=':
                        printf("\txor\t%s,%s,%s\n", $1, $3, $5);
                        printf("\tseqz\t%s,%s\n", $1, $1);
                        break;
                }
          }
          | REGISTER '=' REGISTER operator integer {
                switch($4[0]){
                    case '+':
                        printf("\tadd\t%s,%s,%d\n", $1, $3, $5);
                        break;
                    case '<':
                        printf("\tslti\t%s,%s,%d\n", $1, $3, $5);
                        break;
                }
          }
          | REGISTER '=' operator REGISTER  {
                switch($3[0]){
                    case '!':
                        printf("\tseqz\t%s,%s\n", $1, $4);
                        printf("\tand\t%s,%s,0xff\n", $1, $1);
                        break;
                    case '-':
                        printf("\tsub\t%s,zero,%s\n", $1, $4);
                        break;
                }
          }
          | REGISTER '=' REGISTER   {printf("\tmv\t%s,%s\n", $1, $3);}
          | REGISTER '[' INT_CONSTANT ']' '=' REGISTER  {printf("\tsw\t%s,%d(%s)\n", $6, $3, $1);}
          | REGISTER '=' REGISTER '[' INT_CONSTANT ']'  {printf("\tlw\t%s,%d(%s)\n", $1, $5, $3);}
          | IF REGISTER operator REGISTER GOTO LABEL    {
                switch($3[0]){
                    case '<':
                        printf("\tblt\t%s,%s,.%s\n", $2, $4, $6);
                        break;
                    case '>':
                        printf("\tbgt\t%s,%s,.%s\n", $2, $4, $6);
                        break;
                    case '!':
                        printf("\tbne\t%s,%s,.%s\n", $2, $4, $6);
                        break;
                    case '=':
                        printf("\tbeq\t%s,%s,.%s\n", $2, $4, $6);
                        break;
                    case '@': //<=
                        printf("\tble\t%s,%s,.%s\n", $2, $4, $6);
                        break;
                    case '#': //>=
                        printf("\tble\t%s,%s,.%s\n", $4, $2, $6);
                        break;
                }
          }
          | GOTO LABEL  {printf("\tj\t.%s\n", $2);}
          | LABEL ':'   {printf(".%s:\n", $1);}
          | CALL FUNCTION   {printf("\tcall\t%s\n", $2);}
          | STORE REGISTER INT_CONSTANT {printf("\tsw\t%s,%d(sp)\n", $2, $3 * 4);}
          | LOAD INT_CONSTANT REGISTER  {printf("\tlw\t%s,%d(sp)\n", $3, $2 * 4);}
          | LOAD GVAR REGISTER  {
                printf("\tlui\t%s,%%hi(%s)\n",$3, $2);
                printf("\tlw\t%s,%%lo(%s)(%s)\n", $3, $2, $3);
          }
          | LOADADDR INT_CONSTANT REGISTER {printf("\tadd\t%s,sp,%d\n", $3, $2 * 4);}
          | LOADADDR GVAR REGISTER {
                printf("\tlui\t%s,%%hi(%s)\n",$3, $2);
                printf("\tadd\t%s,%s,%%lo(%s)\n", $3, $3, $2);
          }
          /*| REGISTER '=' MALLOC INT_CONSTANT {if ($4 % 4 != 0 || $4 == 0) yyerror("what do you mean by this mallocing size?"); stmts.push_back(new stmt_malloc($1, $4));}*/
          ;

%%

void yyerror(const char *s)
{
    printf("line %d: %s\n", yylineno, s);
    exit(1);
}
int main(){
    yyparse();
    return 0;
}
