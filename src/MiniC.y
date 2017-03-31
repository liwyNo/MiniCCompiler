%{

#include <cstdio>
#include "symbol.h"
#include "yaccUtils.h"
#include "control_flow.h>
#include <cstdlib>
#include <cstring>

extern int yylex (void);
void yyerror(const char *s);

%}

%code requires {
#include "yaccType.h"
}

%union {
    char *vstr;
    char vchar;
    int vint;
    double vdouble;
    void *vptr;
    type_qualifier_s_t type_qualifier_s;
    storage_class_specifier_s_t storage_class_specifier_s;
    type_specifier_s_t type_specifier_s;
    struct_or_union_s_t struct_or_union_s;
    struct_or_union_specifier_s_t struct_or_union_specifier_s;
    specifier_qualifier_list_s_t specifier_qualifier_list_s;
    declaration_specifiers_s_t declaration_specifiers_s;
    init_declarator_list_i_t initializer_list_i;
    init_declarator_i_t init_declarator_i;
    enum_specifier_s_t enum_specifier_s;
    enumerator_list_s_t enumerator_list_s;
    enumerator_s_t enumerator_s;
    expression_s_t expression_s;
    pointer_s_t pointer_s;
    direct_declarator_s_t direct_declarator_s;
    identifier_list_s_t *identifier_list_s;
    parameter_list_s_t *parameter_list_s;
    declarator_s_t declarator_s;

}

%token <vstr> IDENTIFIER
%token <vstr> STR_CONSTANT
%token <vchar> CHAR_CONSTANT
%token <vint> INT_CONSTANT
%token <vint> ENUM_CONSTANT
%token <vdouble> DOUBLE_CONSTANT
%token SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <vptr> TYPE_NAME
%token TYPEDEF
%token STATIC
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOID
%token STRUCT UNION ENUM
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <expression_s_t> primary_expression
%type <expression_s_t> constant
%type <expression_s_t> expression
%type <expression_s_t> postfix_expression
%type <expression_s_t> string
%type <expression_s_t> unary_expression
%type <expression_s_t> cast_expression
%type <expression_s_t> multiplicative_expression
%type <expression_s_t> additive_expression
%type <expression_s_t> shift_expression
%type <expression_s_t> relational_expression
%type <expression_s_t> equality_expression
%type <expression_s_t> and_expression
%type <expression_s_t> exclusive_or_expression
%type <expression_s_t> inclusive_or_expression
%type <expression_s_t> logical_and_expression
%type <expression_s_t> logical_or_expression
%type <expression_s_t> conditional_expression
%type <expression_s_t> assignment_expression

%type <type_qualifier_s> type_qualifier
%type <storage_class_specifier_s> storage_class_specifier
%type <type_specifier_s> type_specifier
%type <struct_or_union_s> struct_or_union
%type <struct_or_union_specifier_s> struct_or_union_specifier
%type <specifier_qualifier_list_s> specifier_qualifier_list
%type <declaration_specifiers_s> declaration_specifiers
%type <enum_specifier_s> enum_specifier
%type <enumerator_list_s> enumerator_list
%type <enumerator_s> enumerator
%type <expression_s> constant_expression
%type <pointer_s> pointer
%type <direct_declarator_s> direct_declarator
%type <identifier_list_s> identifier_list
%type <parameter_list_s> parameter_list
%type <declarator_s> declarator

%nonassoc IFX
%nonassoc ELSE

%start translation_unit

%%
primary_expression:
	  IDENTIFIER
	| constant
	| string
	| '(' expression ')'
	;

constant:
	  INT_CONSTANT		{

		}
  | CHAR_CONSTANT
	| DOUBLE_CONSTANT
	| ENUM_CONSTANT
	;

string:
	  STR_CONSTANT		{
				char temp_name = new char[7];
				sprintf(temp_name,"c%d",CreateConstant());
				gen_const("str",temp_name,$1);
				$$.value.vstr = $1;
				$$.truelist = GotoList()
			}
	;

postfix_expression:
	  primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	| '(' type_name ')' '{' initializer_list '}'
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list:
	  assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression:
	  postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

unary_operator:
	  '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression:
	  unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression:
	  cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression:
	  multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression:
	  additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression:
	  shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression:
	  relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression:
	  equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression:
	  and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression:
	  exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression:
	  inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression:
	  logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression:
	  logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression:
	  conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator:
	  '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression:
	  assignment_expression
	| expression ',' assignment_expression
	;

constant_expression:
	  conditional_expression
	;

storage_class_specifier:
	  TYPEDEF   {$$.hasTYPEDEF = 1; $$.hasSTATIC = 0;}
	| STATIC    {$$.hasTYPEDEF = 0; $$.hasSTATIC = 1;}
	;

type_qualifier:
	  CONST {$$.hasCONST = 1;}
	;

type_specifier:
	  VOID                      {$$.type = (const_Typename_ptr)LookupSymbol("void", NULL); $$.sign=-1;}
	| CHAR                      {$$.type = (const_Typename_ptr)LookupSymbol("char", NULL); $$.sign=-1;}
	| SHORT                     {$$.type = (const_Typename_ptr)LookupSymbol("short", NULL); $$.sign=-1;}
	| INT                       {$$.type = (const_Typename_ptr)LookupSymbol("int", NULL); $$.sign=-1;}
	| LONG                      {$$.type = (const_Typename_ptr)LookupSymbol("long", NULL); $$.sign=-1;}
	| FLOAT                     {$$.type = (const_Typename_ptr)LookupSymbol("float", NULL); $$.sign=-1;}
	| DOUBLE                    {$$.type = (const_Typename_ptr)LookupSymbol("double", NULL); $$.sign=-1;}
	| SIGNED                    {$$.type = (const_Typename_ptr)LookupSymbol("int", NULL); $$.sign=1;}
	| UNSIGNED                  {$$.type = (const_Typename_ptr)LookupSymbol("char", NULL); $$.sign=0;}
	| struct_or_union_specifier {$$.type = $1; $$.sign=-1;}
	| enum_specifier            {$$.type = $1; $$.sign=-1;}
	| TYPE_NAME                 {$$.type = (const_Typename_ptr)$1; $$.sign=-1;}
	;

declaration_specifiers:
	  storage_class_specifier declaration_specifiers    {$$ = $2; $$.hasTYPEDEF |= $1.hasTYPEDEF; $$.hasSTATIC |= $1.hasSTATIC;}
	| storage_class_specifier                           {$$.hasTYPEDEF = $1.hasTYPEDEF; $$.hasSTATIC = $1.hasSTATIC; $$.hasCONST = 0; $$.sign = -1; $$.type = NULL;}
	| type_specifier declaration_specifiers             {$$ = $2; TypeCombine($1.sign, $1.type, &($$.sign), &($$.type));}
	| type_specifier                                    {$$.hasTYPEDEF=$$.hasSTATIC=$$.hasCONST=0; $$.sign = $1.sign; $$.type = $1.type;}
	| type_qualifier declaration_specifiers             {$$ = $2; $$.hasCONST |= $1.hasCONST;}
	| type_qualifier                                    {$$.hasTYPEDEF=$$.hasSTATIC=0; $$.hasCONST = $1.hasCONST; $$.sign = -1; $$.type = NULL;}
	;

init_declarator:
	  declarator '=' initializer
	| declarator
	;

init_declarator_list:
	  init_declarator
	| init_declarator_list ',' init_declarator
	;

declaration:
	  declaration_specifiers ';'                        {/* nothing to do */}
	| declaration_specifiers init_declarator_list ';'   {}
	;

struct_or_union:
	  STRUCT    {$$.hasSTRUCT=1;$$.hasUNION=0;}
	| UNION     {$$.hasSTRUCT=0;$$.hasUNION=1;}
	;

struct_or_union_specifier:
	  struct_or_union '{' struct_declaration_list '}'               {}
	| struct_or_union IDENTIFIER '{' struct_declaration_list '}'    {}
	| struct_or_union IDENTIFIER                                    {
            Typename_t *t = (Typename_t*)malloc(sizeof(Typename_t));
            if ($1.hasSTRUCT)
                t->type = idt_struct;
            else if ($1.hasUNION)
                t->type = idt_union;
            t->name = $2;
            t->structure = NULL;
            StackAddTypename(t);
            $$ = t;
        }
	;

specifier_qualifier_list:
	  type_specifier specifier_qualifier_list   {$$ = $2; TypeCombine($1.sign, $1.type, &($$.sign), &($$.type));}
	| type_specifier                            {$$.hasCONST=0; $$.sign=$1.sign; $$.type=$1.type;}
	| type_qualifier specifier_qualifier_list   {$$ = $2; $$.hasCONST = $1.hasCONST;}
	| type_qualifier                            {$$.hasCONST = $1.hasCONST; $$.sign = -1; $$.type = NULL;}
	;

struct_declarator_list:
	  declarator
	| struct_declarator_list ',' declarator
	;

struct_declaration:
	  specifier_qualifier_list ';'
	| specifier_qualifier_list struct_declarator_list ';'
	;

struct_declaration_list:
	  struct_declaration
	| struct_declaration_list struct_declaration
	;

enum_specifier:
	  ENUM '{' enumerator_list '}'                  { $$ = newEnum(NULL, $3); }
	| ENUM '{' enumerator_list ',' '}'              { $$ = newEnum(NULL, $3); }
	| ENUM IDENTIFIER '{' enumerator_list '}'       { $$ = newEnum($2, $4); }
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'   { $$ = newEnum($2, $4); }
	| ENUM IDENTIFIER                               {
            Typename_t *t = (Typename_t*)malloc(sizeof(Typename_t));
            t->type = idt_enum;
            t->name = $2;
            t->structure = NULL;
            StackAddTypename(t);
            $$ = t;
        }
	;

enumerator_list:
	  {$<vint>$=0;} enumerator           {$$=(EnumTable_t*)malloc(sizeof(EnumTable_t)); $$->name=$2.name; $$->value=$2.value; $$->next=NULL;}
	| enumerator_list ',' {$<vint>$=($<enumerator_list_s>-1)->value + 1;} enumerator    {
            $$=(EnumTable_t*)malloc(sizeof(EnumTable_t));
            $$->name = $4.name;
            $$->value = $4.value;
            $$->next = $1;
        }
	;

enumerator:
	  IDENTIFIER '=' constant_expression    {$$.name=$1; $$.value=$3.value.vint;
                                                if (!$3.isConst||$3.type!=idt_int) yyerror("enumerator not integer constant");}
	| IDENTIFIER                            {$$.name=$1; $$.value=$<vint>0;}
	;

direct_declarator:
	  IDENTIFIER                                    {$$.type=1; $$.data.d1=$1;}
	| '(' declarator ')'                            {$$.type=2; $$.data.d2=$2;}
	| direct_declarator '[' ']'                     {$$.type=3; $$.data.d3=(direct_declarator_s_t*)memDup(&$1, sizeof(direct_declarator_s_t)); freeDD(&$1);}
	| direct_declarator '[' constant_expression ']' {$$.type=4; if (!$3.isConst||$3.type!=idt_int) yyerror("array declaration not integer constant"); $$.data.d4.dd=(direct_declarator_s_t*)memDup(&$1, sizeof(direct_declarator_s_t)); freeDD(&$1); $$.data.d4.ce=$3.value.vint;}
	| direct_declarator '(' parameter_list ')'      {$$.type=5; $$.data.d5.dd=(direct_declarator_s_t*)memDup(&$1,sizeof(direct_declarator_s_t)); freeDD(&$1); $$.data.d5.pl=$3;}
	| direct_declarator '(' ')'                     {$$.type=6; $$.data.d6=(direct_declarator_s_t*)memDup(&$1,sizeof(direct_declarator_s_t)); freeDD(&$1);}
	| direct_declarator '(' identifier_list ')'     {$$.type=7; $$.data.d7.dd=(direct_declarator_s_t*)memDup(&$1,sizeof(direct_declarator_s_t)); freeDD(&$1); $$.data.d7.il=$3;}
	;

pointer:
	  '*' type_qualifier pointer    {$$=(pointer_s_t)malloc(sizeof(pointer_list_t)); $$->hasConst=$2.hasCONST; $$->next=$3;}
	| '*' type_qualifier            {$$=(pointer_s_t)malloc(sizeof(pointer_list_t)); $$->hasConst=$2.hasCONST; $$->next=NULL;}
	| '*' pointer                   {$$=(pointer_s_t)malloc(sizeof(pointer_list_t)); $$->hasConst=0; $$->next=$2;}
	| '*'                           {$$=(pointer_s_t)malloc(sizeof(pointer_list_t)); $$->hasConst=0; $$->next=NULL;}
	;

declarator:
	  pointer direct_declarator {$$.ptr=$1; $$.dd=(direct_declarator_s_t*)memDup(&$2,sizeof(direct_declarator_s_t)); freeDD(&$2);}
	| direct_declarator         {$$.dd=(direct_declarator_s_t*)memDup(&$1,sizeof(direct_declarator_s_t)); freeDD(&$1);}
	;

parameter_declaration:
	  declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

parameter_list:
	  parameter_declaration
	| parameter_list ',' parameter_declaration
	;

identifier_list:
	  IDENTIFIER                        {$$=(identifier_list_s_t*)malloc(sizeof(identifier_list_s_t)); $$->next=NULL; $$->id=$1;}
	| identifier_list ',' IDENTIFIER    {$$=(identifier_list_s_t*)malloc(sizeof(identifier_list_s_t)); $$->next=$1; $$->id=$3;}
	;

type_name:
	  specifier_qualifier_list abstract_declarator
	| specifier_qualifier_list
	;

abstract_declarator:
	  pointer direct_abstract_declarator
	| pointer
	| direct_abstract_declarator
	;

direct_abstract_declarator:
	  '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_list ')'
	;

designator: /* not supported */
	  '[' constant_expression ']'
	| '.' IDENTIFIER
	;

initializer:
	  '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	| assignment_expression
	;

initializer_list:
	  designator initializer    {yyerror("designator is not supported yet");}
	| initializer
	| initializer_list ',' designator initializer
	| initializer_list ',' initializer
	;

statement:
	  labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement:
	  IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

block_item:
	  declaration
	| statement
	;

block_item_list:
	  block_item
	| block_item_list block_item
	;

compound_statement:
	  '{' '}'
	| '{'  block_item_list '}'
	;

expression_statement:
	  ';'
	| expression ';'
	;

selection_statement:
	  IF '(' expression ')' statement ELSE statement
	| IF '(' expression ')' statement %prec IFX
	| SWITCH '(' expression ')' statement
	;

iteration_statement:
	  WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	| FOR '(' declaration expression_statement ')' statement
	| FOR '(' declaration expression_statement expression ')' statement
	;

jump_statement:
	  GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit:
	  external_declaration                  {}
	| translation_unit external_declaration {}
	;

external_declaration:
	  function_definition   {}
	| declaration           {}
	;

function_definition:
	  declaration_specifiers declarator declaration_list compound_statement {yyerror("not support this type of function definition");}
	| declaration_specifiers declarator compound_statement
	;

declaration_list: /* this is not supported */
	  declaration
	| declaration_list declaration
	;
%%

void yyerror(const char *s)
{
    puts(s);
}
