%{

#include <cstdio>
#include "symbol.h"
#include "gen.h"
#include "yaccUtils.h"
#include "expression.h"
#include <cstdlib>
#include <cstring>
#include <string>

int yylex (void);

%}

%code requires {
#include "yaccType.h"
void yyerror(const char *s);
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
    enum_specifier_s_t enum_specifier_s;
    enumerator_list_s_t enumerator_list_s;
    enumerator_s_t enumerator_s;
    expression_s_t expression_s;
    pointer_s_t pointer_s;
    direct_declarator_s_t direct_declarator_s;
    parameter_list_s_t parameter_list_s;
    declarator_s_t declarator_s;
    struct_declarator_list_s_t *struct_declarator_list_s;
    abstract_declarator_s_t abstract_declarator_s;
    direct_abstract_declarator_s_t direct_abstract_declarator_s;
    type_name_s_t type_name_s;
    initializer_list_s_t *initializer_list_s;
    initializer_s_t initializer_s;
    init_declarator_s_t init_declarator_s;
    init_declarator_list_s_t *init_declarator_list_s;
    statement_i_t statement_i;
    statement_s_t statement_s;
    expression_statement_s_t expression_statement_s;
    for_jumper2_s_t for_jumper2_s;
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

%type <expression_s> constant_expression
%type <expression_s> primary_expression
%type <expression_s> constant
%type <expression_s> expression
%type <expression_s> postfix_expression
%type <expression_s> string
%type <expression_s> unary_expression
%type <expression_s> cast_expression
%type <expression_s> multiplicative_expression
%type <expression_s> additive_expression
%type <expression_s> shift_expression
%type <expression_s> relational_expression
%type <expression_s> equality_expression
%type <expression_s> and_expression
%type <expression_s> exclusive_or_expression
%type <expression_s> inclusive_or_expression
%type <expression_s> logical_and_expression
%type <expression_s> logical_or_expression
%type <expression_s> conditional_expression
%type <expression_s> assignment_expression
%type <vint> assignment_operator 

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
%type <pointer_s> pointer
%type <direct_declarator_s> direct_declarator
%type <parameter_list_s> parameter_list
%type <declarator_s> declarator
%type <struct_declarator_list_s> struct_declarator_list
%type <abstract_declarator_s> abstract_declarator
%type <direct_abstract_declarator_s> direct_abstract_declarator
%type <type_name_s> type_name
%type <initializer_list_s> initializer_list
%type <initializer_s> initializer
%type <init_declarator_s> init_declarator
%type <init_declarator_list_s> init_declarator_list
%type <vint> jumper for_jumper1
%type <for_jumper2_s> for_jumper2
%type <expression_statement_s> expression_statement
%type <statement_s> statement labeled_statement compound_statement selection_statement iteration_statement jump_statement
%type <statement_s> block_item block_item_list
%type <statement_i> if_statement_inherit

%nonassoc IFX
%nonassoc ELSE

%start translation_unit

%%
primary_expression:
	  IDENTIFIER			{
            int symbol_type;
            void *sym_ptr;
            sym_ptr = LookupSymbol($1,&symbol_type);
            if(symbol_type == 0)//啥都不是，返回错误
            {
                yyerror("Symbol hasn't been declared!");
            }
            else if(symbol_type == IDENTIFIER)//没有其他可能了，不可能是enum_constant或者type,在lex时就被筛过了
            {
				Identifier_t *id = (Identifier_t *)sym_ptr;
                $$.isConst = id -> isConst;
				#warning "The IDENTIFIER hasn't finished yet!"
				//if($$.isConst)
					//$$.value = id -> value;
                $$.lr_value = 0;
				$$.addr = id -> TACname;
				$$.laddr = NULL;
				$$.type = id -> type;
            }
        }
	| constant		{
		$$ = $1;
	}
	| string		{
		$$ = $1;
	}
	| '(' expression ')'	{
		$$ = $2;
	}
	;

constant:
	INT_CONSTANT		{
		$$.lr_value=1;
		$$.isConst = 1;
		$$.value.vint = $1;
		$$.addr = strdup(('c' + std::to_string(CreateConstant())).c_str());
		$$.laddr = NULL;
		$$.type = (const_Typename_ptr)LookupSymbol("int", NULL);
		gen_const("int4",$$.addr,&$1);
	}
  	| CHAR_CONSTANT		{
		$$.lr_value = 1;
		$$.isConst = 1;
		//$$.value.vchar = $1;
		$$.addr = strdup(('c' + std::to_string(CreateConstant())).c_str());
		$$.laddr = NULL;
		$$.type = (const_Typename_ptr)LookupSymbol("char", NULL);
		gen_const("int1",$$.addr,&$1);
	}
	| DOUBLE_CONSTANT	{
		$$.lr_value = 1;
		$$.isConst = 1;
		//$$.value.vdouble = $1;
		$$.addr = strdup(('c' + std::to_string(CreateConstant())).c_str());
		$$.laddr = NULL;
		$$.type = (const_Typename_ptr)LookupSymbol("double", NULL);
		gen_const("float8",$$.addr,&$1); //应该翻译成三地址码中的double!
	}
	| ENUM_CONSTANT		{
		$$.lr_value = 1;
		$$.isConst = 1;
		$$.value.vint = $1;
		$$.addr = strdup(('c' + std::to_string(CreateConstant())).c_str());
		$$.laddr = NULL;
		$$.type = (const_Typename_ptr)LookupSymbol("int", NULL);
		gen_const("int4",$$.addr,&$1);
	}
	;

string:
	  STR_CONSTANT		{
				//$$.value.vstr = $1;
				$$.lr_value = 1;
				$$.addr = strdup(('c' + std::to_string(CreateConstant())).c_str());
				$$.laddr = NULL;
				$$.isConst = 1;			
				Typename_t *tmp_type = new Typename_t;
				tmp_type -> type = idt_array;
				tmp_type -> name = NULL;
				tmp_type -> isConst = 1;
				tmp_type -> size = strlen($1) + 1;
				tmp_type -> structure = new IdStructure_t;
				tmp_type -> structure -> pointer.rbase_type = (const_Typename_ptr)LookupSymbol("char", NULL);
				tmp_type -> structure -> pointer.base_type = (const_Typename_ptr)LookupSymbol("char", NULL);
				tmp_type -> structure -> pointer.length = strlen($1) + 1;
				$$.type = tmp_type;
				gen_const("str",$$.addr,$1);
			}
	;

postfix_expression:
	  primary_expression	{
		  $$ = $1;
	  }
	| postfix_expression '[' expression ']'	{
		if($3.type -> type < 8) //下标必须是整数
		{
			if($1.lr_value == 0 && ($1.type -> type == idt_array || $1.type -> type == idt_pointer))
			{
				char *tmp_name1, *tmp_name2, *tmp_name3;
				//直接利用如下函数
				tmp_name1 = get_cast_name(idt_int, $3.type -> type, $3.get_addr());
				
				const_Typename_ptr b_type = $1.type -> structure -> pointer . base_type;
				
				tmp_name2 = sizeof_type(b_type);
				tmp_name3 = get_TAC_name('t',CreateTempVar());
				gen_cpy(tmp_name3,tmp_name2); //这么一大坨终于得到该指针这一维的大小了。。。

				gen_op2(tmp_name2,tmp_name2,tmp_name1,"*");
				char *pf = $1.get_addr();
				gen_op2(pf, pf, tmp_name2,"+");

				$$.type = b_type;
				$$.addr = NULL;
				$$.laddr = pf;
				if($$.type -> type == idt_array) //假如这是个数组，则不能修改，是个右值
					$$.lr_value = 1;
				else	
					$$.lr_value = 0;
				$$.isConst = 0; //只要是这种带指针的统统不算常量表达式
			}
			else yyerror("only the pointer or array can use [] operator !");
		}
		else yyerror("The subscript must be integer!");

	}
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP	{
		postfix_expression_INC_DEC_OP($$,$1,"+");
	}
	| postfix_expression DEC_OP {
		postfix_expression_INC_DEC_OP($$,$1,"-");
	}
	| '(' type_name ')' '{' initializer_list '}'        /*{
            initializer_s_t *tmp = new initializer_s_t;
            tmp->addr = NULL;
            tmp->lst = $5;
            int tv = CreateTempVar();
            char *TACname = strdup(('t' + std::to_string(tv)).c_str());
            genDeclare($2->type, TACname, false);
            genInitilize($2->type, TACname, tmp);
            freeInit(tmp);
            $$.isConst = 1;
            $$.type = $2->type;
            $$.addr = TACname;
        }*/
	| '(' type_name ')' '{' initializer_list ',' '}'
	;

argument_expression_list:
	  assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression:
	  postfix_expression		{$$ = $1;}
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
	  unary_expression			{$$ = $1;}
	| '(' type_name ')' cast_expression
	;

multiplicative_expression:
	  cast_expression           {$$ = $1;}
	| multiplicative_expression '*' cast_expression			{get_ADD_SUB_MUL_DIV($$,$1,$3,"+");}
	| multiplicative_expression '/' cast_expression			{get_ADD_SUB_MUL_DIV($$,$1,$3,"+");}
	| multiplicative_expression '%' cast_expression
	;

additive_expression:
	  multiplicative_expression {$$ = $1;}
	| additive_expression '+' multiplicative_expression		{get_ADD_SUB_MUL_DIV($$,$1,$3,"+");}
	| additive_expression '-' multiplicative_expression		{get_ADD_SUB_MUL_DIV($$,$1,$3,"+");}
	;

shift_expression:
	  additive_expression       {$$ = $1;}
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression:
	  shift_expression          {$$ = $1;}
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression:
	  relational_expression     {$$ = $1;}
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression:
	  equality_expression       {$$ = $1;}
	| and_expression '&' equality_expression
	;

exclusive_or_expression:
	  and_expression            {$$ = $1;}
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression:
	  exclusive_or_expression   {$$ = $1;}
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression:
	  inclusive_or_expression   {$$ = $1;}
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression:
	  logical_and_expression    {$$ = $1;}
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression:
	  logical_or_expression     {$$ = $1;}
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression:
	  conditional_expression    {$$ = $1;}
	| unary_expression assignment_operator assignment_expression {
		if($2 == 0)
		{
			$$ = get_assign($1,$3);	
		}
		if($2 == ADD_ASSIGN)
		{
			expression_s_t tmp;
		}
    }
	;

assignment_operator:
	  '='			{
		  $$ = 0; //0 表示等号
	  }
	| MUL_ASSIGN	{$$ = MUL_ASSIGN;}
	| DIV_ASSIGN	{$$ = DIV_ASSIGN;}
	| MOD_ASSIGN	{$$ = MOD_ASSIGN;}
	| ADD_ASSIGN	{$$ = ADD_ASSIGN;}
	| SUB_ASSIGN	{$$ = SUB_ASSIGN;}
	| LEFT_ASSIGN	{$$ = LEFT_ASSIGN;}
	| RIGHT_ASSIGN	{$$ = RIGHT_ASSIGN;}
	| AND_ASSIGN	{$$ = AND_ASSIGN;}
	| XOR_ASSIGN	{$$ = XOR_ASSIGN;}
	| OR_ASSIGN		{$$ = OR_ASSIGN;}
	;

expression:
	  assignment_expression     {$$ = $1;}
	| expression ',' assignment_expression  {$$ = $3;}
	;

constant_expression:
	  conditional_expression    {$$ = $1;}
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
	| UNSIGNED                  {$$.type = (const_Typename_ptr)LookupSymbol("uint", NULL); $$.sign=0;}
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
	  declarator '=' initializer    {$$.decl = $1; $$.init = memDup(&$3);}
	| declarator                    {$$.decl = $1; $$.init = NULL;}
	;

init_declarator_list:
	  init_declarator               {$$=new init_declarator_list_s_t; $$->idecl=$1; $$->next=NULL;}
	| init_declarator_list ',' init_declarator  {$$=new init_declarator_list_s_t; $$->idecl=$3; $$->next=$1;}
	;

declaration:
	  declaration_specifiers ';'                        /* nothing to do */
	| declaration_specifiers init_declarator_list ';'   {
            if ($1.type==NULL)yyerror("declaration error");
            if ($1.hasTYPEDEF && $1.hasSTATIC) yyerror("typedef with static");
            const_Typename_ptr tmptype = $1.type;
            if ($1.hasCONST)
            {
                Typename_t *tmp = memDup($1.type);
                tmp->isConst = 1;
                tmptype = tmp;
            }
            for (init_declarator_list_s_t *i = $2; i; i = i->next) {
                Identifier_t *id = StackDeclare(makeType(tmptype, i->idecl.decl), $1.hasSTATIC, $1.hasTYPEDEF, getDeclaratorName(&(i->idecl.decl)));
                if (!$1.hasTYPEDEF) {
                    genDeclare(id->type, id->TACname, symbolStack->next == NULL || $1.hasSTATIC);
                    genInitilize(id->type, id->TACname, i->idecl.init);
                    if (i->idecl.init->addr && id->type->isConst) {
                        id->isConst = 1;
                        id->value = i->idecl.init->value;
                    }
                    else
                        id->isConst = 0;
                } else
                    if (i->idecl.init)
                        yyerror("typedef with initial value");
            }
            freeIDL($2);
        }
	;

struct_or_union:
	  STRUCT    {$$.hasSTRUCT=1;$$.hasUNION=0;}
	| UNION     {$$.hasSTRUCT=0;$$.hasUNION=1;}
	;

struct_or_union_specifier:
	  struct_or_union struct_push_symbol_stack '{' struct_declaration_list '}'               {
            Typename_t *t = newStructUnion($1.hasSTRUCT, NULL, true);
            symbolStack = symbolStack->next;
            StackAddTypename(t);
            $$ = t;
        }
	| struct_or_union IDENTIFIER struct_push_symbol_stack '{' struct_declaration_list '}'    {
            Typename_t *t = newStructUnion($1.hasSTRUCT, $2, true);
            symbolStack = symbolStack->next;
            StackAddTypename(t);
            $$ = t;
        }
    | struct_or_union struct_push_symbol_stack '{' '}'    {
            Typename_t *t = newStructUnion($1.hasSTRUCT, NULL, true);
            symbolStack = symbolStack->next;
            StackAddTypename(t);
            $$ = t;
        }
    | struct_or_union IDENTIFIER struct_push_symbol_stack '{' '}'    {
            Typename_t *t = newStructUnion($1.hasSTRUCT, $2, true);
            symbolStack = symbolStack->next;
            StackAddTypename(t);
            $$ = t;
        }
	| struct_or_union IDENTIFIER                                    {
            Typename_t *t = newStructUnion($1.hasSTRUCT, $2, false);
            StackAddTypename(t);
            $$ = t;
        }
	;

struct_push_symbol_stack:   {PushSymbolStack(0);}

specifier_qualifier_list:
	  type_specifier specifier_qualifier_list   {$$ = $2; TypeCombine($1.sign, $1.type, &($$.sign), &($$.type));}
	| type_specifier                            {$$.hasCONST=0; $$.sign=$1.sign; $$.type=$1.type;}
	| type_qualifier specifier_qualifier_list   {$$ = $2; $$.hasCONST = $1.hasCONST;}
	| type_qualifier                            {$$.hasCONST = $1.hasCONST; $$.sign = -1; $$.type = NULL;}
	;

struct_declarator_list:
	  declarator                            {$$=new struct_declarator_list_s_t;$$->next=NULL;$$->decl=$1;}
	| struct_declarator_list ',' declarator {$$=new struct_declarator_list_s_t;$$->next=$1;$$->decl=$3;}
	;

struct_declaration:
	  specifier_qualifier_list ';'                          {if ($1.type==NULL)yyerror("struct declaration error");}
	| specifier_qualifier_list struct_declarator_list ';'   {
            if ($1.type==NULL)yyerror("struct declaration error");
            const_Typename_ptr tmptype = $1.type;
            if ($1.hasCONST)
            {
                Typename_t *tmp = memDup($1.type);
                tmp->isConst = 1;
                tmptype = tmp;
            }
            for (struct_declarator_list_s_t *i = $2; i; i = i->next)
                StackDeclare(makeType(tmptype, i->decl), 0, 0, getDeclaratorName(&(i->decl)));
            freeSDL($2);
        }
	;

struct_declaration_list:    /* nothing to do */
	  struct_declaration
	| struct_declaration_list struct_declaration
	;

enum_specifier:
	  ENUM '{' enumerator_list '}'                  { $$ = newEnum(NULL, $3); StackAddEnumTable($3); }
	| ENUM '{' enumerator_list ',' '}'              { $$ = newEnum(NULL, $3); StackAddEnumTable($3); }
	| ENUM IDENTIFIER '{' enumerator_list '}'       { $$ = newEnum($2, $4); StackAddEnumTable($4); }
	| ENUM IDENTIFIER '{' enumerator_list ',' '}'   { $$ = newEnum($2, $4); StackAddEnumTable($4); }
	| ENUM IDENTIFIER                               {
            Typename_t *t = new Typename_t;
            t->type = idt_int; // idt_enum is replaced by idt_int
            t->name = $2;
            t->structure = NULL;
            StackAddTypename(t);
            t->size = -1;
            $$ = t;
        }
	;

enumerator_list:
	  {$<vint>$=0;} enumerator           {$$=new EnumTable_t; $$->name=$2.name; $$->value=$2.value; $$->next=NULL;}
	| enumerator_list ',' {$<vint>$=($<enumerator_list_s>-1)->value + 1;} enumerator    {
            $$=new EnumTable_t;
            $$->name = $4.name;
            $$->value = $4.value;
            $$->next = $1;
        }
	;

enumerator:
	  IDENTIFIER '=' constant_expression    {$$.name=$1; $$.value=$3.value.vint;
                                                if (!$3.isConst||$3.type->type!=idt_int) yyerror("enumerator not integer constant");}
	| IDENTIFIER                            {$$.name=$1; $$.value=$<vint>0;}
	;

direct_declarator:
	  IDENTIFIER                                    {$$.type=1; $$.data.d1=$1;}
	| '(' declarator ')'                            {$$.type=2; $$.data.d2=$2;}
	| direct_declarator '[' ']'                     {$$.type=3; $$.data.d3=memDup(&$1);}
	| direct_declarator '[' constant_expression ']' {$$.type=4; if (!$3.isConst||$3.type->type!=idt_int) yyerror("array declaration not integer constant"); $$.data.d4.dd=memDup(&$1); $$.data.d4.ce=$3.value.vint;}
	| direct_declarator '(' parameter_list ')'      {$$.type=5; $$.data.d5.dd=memDup(&$1); $$.data.d5.pl=$3; symbolStack = symbolStack->next;}
	| direct_declarator '(' ')'                     {$$.type=6; $$.data.d6=memDup(&$1);}
	| direct_declarator '(' identifier_list ')'     {yyerror("not support for this type of function declaration");}
	;

pointer:
	  '*' type_qualifier pointer    {$$=new pointer_list_t; $$->hasConst=$2.hasCONST; $$->next=$3;}
	| '*' type_qualifier            {$$=new pointer_list_t; $$->hasConst=$2.hasCONST; $$->next=NULL;}
	| '*' pointer                   {$$=new pointer_list_t; $$->hasConst=0; $$->next=$2;}
	| '*'                           {$$=new pointer_list_t; $$->hasConst=0; $$->next=NULL;}
	;

declarator:
	  pointer direct_declarator {$$.ptr=$1; $$.dd=memDup(&$2);}
	| direct_declarator         {$$.ptr=NULL; $$.dd=memDup(&$1);}
	;

parameter_declaration:
	  declaration_specifiers declarator             {
            if ($1.hasTYPEDEF || $1.hasSTATIC)
                yyerror("typedef/static in parameter");
            if ($1.hasCONST) {
                Typename_t *p = memDup($1.type);
                p->isConst = 1;
                StackDeclare(makeType(p, $2), $1.hasSTATIC, $1.hasTYPEDEF, getDeclaratorName(&$2));
            }
            else
                StackDeclare(makeType($1.type, $2), $1.hasSTATIC, $1.hasTYPEDEF, getDeclaratorName(&$2));
        }
	| declaration_specifiers abstract_declarator    {
            if ($1.hasTYPEDEF || $1.hasSTATIC)
                yyerror("typedef/static in parameter");
            if ($1.hasCONST) {
                Typename_t *p = memDup($1.type);
                p->isConst = 1;
                StackDeclare(makeType(p, $2), $1.hasSTATIC, $1.hasTYPEDEF, NULL);
            }
            else
                StackDeclare(makeType($1.type, $2), $1.hasSTATIC, $1.hasTYPEDEF, NULL);
        }
	| declaration_specifiers    {
            if ($1.hasTYPEDEF || $1.hasSTATIC)
                yyerror("typedef/static in parameter");
            if ($1.hasCONST) {
                Typename_t *p = memDup($1.type);
                p->isConst = 1;
                StackDeclare(p, $1.hasSTATIC, $1.hasTYPEDEF, NULL);
            }
            else
                StackDeclare($1.type, $1.hasSTATIC, $1.hasTYPEDEF, NULL);
        }
	;

parameter_list:
	  {PushSymbolStack(0);} parameter_declaration    {$$=symbolStack;}
	| parameter_list ',' parameter_declaration      {$$=$1;}
	;

identifier_list: /* not support */
	  IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name:
	  specifier_qualifier_list abstract_declarator  {
            if ($1.hasCONST) {
                Typename_t *p = memDup($1.type);
                p->isConst = 1;
                $$ = makeType(p, $2);
            }
            else
                $$ = makeType($1.type, $2);
        }
	| specifier_qualifier_list                      {
            if ($1.hasCONST) {
                Typename_t *p = memDup($1.type);
                p->isConst = 1;
                $$ = p;
            }
            else
                $$ = $1.type;
        }
	;

abstract_declarator:
	  pointer direct_abstract_declarator    {$$.ptr = $1; $$.dad = memDup(&$2);}
	| pointer                               {$$.ptr = $1; $$.dad = NULL;}
	| direct_abstract_declarator            {$$.ptr = NULL; $$.dad = memDup(&$1);}
	;

direct_abstract_declarator:
	  '(' abstract_declarator ')'   {$$.type=1; $$.data.d1=$2;}
	| '[' ']'                       {$$.type=2;}
	| '[' constant_expression ']'   {$$.type=3; if (!$2.isConst || $2.type->type != idt_int) yyerror("array declaration not integer constant"); $$.data.d3=$2.value.vint;}
	| direct_abstract_declarator '[' ']'    {$$.type=4; $$.data.d4=memDup(&$1);}
	| direct_abstract_declarator '[' constant_expression ']'    {$$.type=5; $$.data.d5.dad=memDup(&$1); if (!$3.isConst || $3.type->type != idt_int) yyerror("array declaration not integer constant"); $$.data.d5.ce=$3.value.vint;}
	| '(' ')'                       {$$.type=6;}
	| '(' parameter_list ')'        {$$.type=7; $$.data.d7=$2; symbolStack = symbolStack->next;}
	| direct_abstract_declarator '(' ')'    {$$.type=8; $$.data.d8=memDup(&$1);}
	| direct_abstract_declarator '(' parameter_list ')' {$$.type=9; $$.data.d9.dad=memDup(&$1); $$.data.d9.pl=$3; symbolStack = symbolStack->next;}
	;

designator: /* not supported */
	  '[' constant_expression ']'
	| '.' IDENTIFIER
	;

initializer:
	  '{' initializer_list '}'      {$$.lst=$2;$$.addr=NULL;}
	| '{' initializer_list ',' '}'  {$$.lst=$2;$$.addr=NULL;}
    | '{' '}'                       {$$.lst=NULL;$$.addr=NULL;}
	| assignment_expression         {$$.lst=NULL;$$.addr=$1.get_addr(); $$.isConst=$1.isConst; $$.value=$1.value;}
	;

initializer_list:
	  designator initializer    {yyerror("designator is not supported");}
	| initializer               {$$=new initializer_list_s_t; $$->data=$1; $$->next=NULL;}
	| initializer_list ',' designator initializer   {yyerror("designator is not supported");}
	| initializer_list ',' initializer  {$$=new initializer_list_s_t; $$->data=$3; $$->next=$1;}
	;

statement: /* auto convert except expression_statement */
	  labeled_statement
	| compound_statement
	| expression_statement  {$$.caseList = NULL;}
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement:
	  /*IDENTIFIER ':' {
            int lbnum = CreateLabel();
            gen_label(lbnum);
            $<statement_i>$ = $<statement_i>0;
        } statement*/
	  CASE constant_expression {gen_label($<vint>$ = CreateLabel());} ':' {$<statement_i>$=$<statement_i>0;} statement   {
            if (!$2.isConst)
                yyerror("case not constant");
            $$.caseList = new CaseList_t;
            $$.caseList->isDefault = 0;
            switch ($2.type->type) {
            case idt_char:
            case idt_uchar:
            case idt_short:
            case idt_ushort:
            case idt_int:
            case idt_uint:
            case idt_long:
            case idt_ulong:
                break;
            default:
                yyerror("case not integer");
            }
            $$.caseList->type = $2.type->type;
            $$.caseList->value = $2.get_addr();
            $$.caseList->label = $<vint>3;
            $$.caseList->next = NULL;
        }
	| DEFAULT {gen_label($<vint>$ = CreateLabel());} ':' {$<statement_i>$=$<statement_i>0;} statement  {
            $$.caseList = new CaseList_t;
            $$.caseList->isDefault = 1;
            $$.caseList->label = $<vint>2;
            $$.caseList->next = NULL;
        }
	;

block_item:
	  declaration   {$$.caseList=NULL;}
	| statement /* auto convert */
	;

block_item_list:
	  block_item /* auto convert */
	| block_item_list {$<statement_i>$ = $<statement_i>0;} block_item   {
            $$.caseList = $3.caseList;
            if ($$.caseList == NULL)
                $$.caseList = $1.caseList;
            else
                $$.caseList->next = $1.caseList;
        }
	;

compound_statement:
	  '{' compound_push_symbol_stack_normal '}'  {
            PopSymbolStack();
            $$.caseList = NULL;
        }
	| '{' compound_push_symbol_stack_normal  {$<statement_i>$ = $<statement_i>0; $<statement_i>$.sblst = NULL;} block_item_list '}' {
            PopSymbolStack();
            $$.caseList = $4.caseList;
        }
	;

compound_push_symbol_stack_normal:   {PushSymbolStack(1); declareParameter($<statement_i>-1.sblst);}
                                 ;

expression_statement:
	  ';'               {$$.have = false;}
	| expression ';'    {$$.have = true; $$.expr = $1;}
	;

selection_statement:
	  IF '(' expression jumper ')' if_statement_inherit statement ELSE {gen_label($4); $<statement_i>$=$<statement_i>0;} statement  {$$.caseList=NULL;}
	| IF '(' expression jumper ')' if_statement_inherit statement %prec IFX {gen_label($4); $$.caseList=NULL;}
	| SWITCH {gen_goto($<vint>$=CreateLabel());} '(' expression ')' {
            $<statement_i>$=$<statement_i>0;
            $<statement_i>$.has_end=1;
            $<statement_i>$.end_num=CreateLabel();
        } statement {
            gen_goto($<statement_i>6.end_num);
            gen_label($<vint>2);
            int default_label = -1;
            for (CaseList_t *i = $7.caseList; i; i = i->next)
                if (i->isDefault)
                    default_label = i->label;
                else
                    genIfGoto($4, i->value, "==", i->label);
            if (default_label != -1)
                gen_goto(default_label);
            gen_label($<statement_i>6.end_num);
            $$.caseList=NULL;
        }
	;

if_statement_inherit:   {$$=$<statement_i>-4;}
                    ;

jumper: { /* $0 must be an expression */
            $$ = CreateLabel();
            genIfGoto($<expression_s>0, "c0", "==", $$);
        }
      ;

iteration_statement:
	  WHILE '(' {
            $<vint>$ = CreateLabel();
            gen_label($<vint>$);
        } expression jumper ')' {
            $<statement_i>$.has_begin = 1;
            $<statement_i>$.has_end = 1;
            $<statement_i>$.begin_num = $<vint>3;
            $<statement_i>$.end_num = $5;
            $<statement_i>$.sblst = NULL;
        } statement   {gen_goto($<vint>3); gen_label($5); $$.caseList=NULL;}
	| DO {
            $<statement_i>$.has_begin = 1;
            $<statement_i>$.has_end = 1;
            $<statement_i>$.begin_num = CreateLabel();
            $<statement_i>$.end_num = CreateLabel();
            $<statement_i>$.sblst = NULL;
            gen_label($<statement_i>$.begin_num);
        } statement WHILE '(' expression ')' ';'    {
            genIfGoto($6, "c0", "!=", $<statement_i>2.begin_num);
            gen_label($<statement_i>2.end_num);
            $$.caseList = NULL;
        }
	| FOR '(' expression_statement for_jumper1 expression_statement ')' {
            $<statement_i>$.has_begin = 1;
            $<statement_i>$.has_end = 1;
            $<statement_i>$.begin_num = $4;
            $<statement_i>$.end_num = CreateLabel();
            $<statement_i>$.sblst = NULL;
            if ($5.have)
                genIfGoto($5.expr, "c0", "==", $<statement_i>$.end_num);
        } statement {
            gen_goto($4);
            gen_label($<statement_i>7.end_num);
            $$.caseList = NULL;
        }
	| FOR '(' expression_statement for_jumper1 expression_statement for_jumper2 expression for_jumper3 ')' {
            $<statement_i>$.has_begin = 1;
            $<statement_i>$.has_end = 1;
            $<statement_i>$.begin_num = $4;
            $<statement_i>$.end_num = $6.lb_end;
            $<statement_i>$.sblst = NULL;
        } statement {
            gen_goto($6.lb_iter);
            gen_label($6.lb_end);
            $$.caseList = NULL;
        }
	| FOR '(' for_push_symbol_stack declaration for_jumper1 expression_statement ')' {
            $<statement_i>$.has_begin = 1;
            $<statement_i>$.has_end = 1;
            $<statement_i>$.begin_num = $5;
            $<statement_i>$.end_num = CreateLabel();
            $<statement_i>$.sblst = NULL;
            if ($6.have)
                genIfGoto($6.expr, "c0", "==", $<statement_i>$.end_num);
        } statement {
            gen_goto($5);
            gen_label($<statement_i>8.end_num);
            $$.caseList = NULL;
            PopSymbolStack();
        }
	| FOR '(' for_push_symbol_stack declaration for_jumper1 expression_statement for_jumper2 expression for_jumper3 ')' {
            $<statement_i>$.has_begin = 1;
            $<statement_i>$.has_end = 1;
            $<statement_i>$.begin_num = $5;
            $<statement_i>$.end_num = $7.lb_end;
            $<statement_i>$.sblst = NULL;
        } statement {
            gen_goto($7.lb_iter);
            gen_label($7.lb_end);
            $$.caseList = NULL;
            PopSymbolStack();
        }
	;

for_push_symbol_stack:  {PushSymbolStack(1);}
                     ;

for_jumper1:    { gen_label($$ = CreateLabel()); }
           ;

for_jumper2:    {
                    $$.lb_end = CreateLabel();
                    if ($<expression_statement_s>0.have)
                        genIfGoto($<expression_statement_s>0.expr, "c0", "==", $$.lb_end);
                    gen_goto($$.lb_state = CreateLabel());
                    gen_label($$.lb_iter = CreateLabel());
                }
           ;

for_jumper3:    {
                    gen_goto($<vint>-3);
                    gen_label(($<for_jumper2_s>-1).lb_state);
                }
           ;

jump_statement:
	  /*GOTO IDENTIFIER ';'   {yyerror("no support for goto");}*/
	  CONTINUE ';'  {
            if ($<statement_i>0.has_begin)
                gen_goto($<statement_i>0.begin_num);
            else
                yyerror("continue error");
            $$.caseList = NULL;
        }
	| BREAK ';'     {
            if ($<statement_i>0.has_end)
                gen_goto($<statement_i>0.end_num);
            else
                yyerror("break; error");
            $$.caseList = NULL;
        }
	| RETURN ';'    { gen_return(NULL); $$.caseList = NULL; }
	| RETURN expression ';' { gen_return($2.get_addr()); $$.caseList = NULL; }
	;

translation_unit:
	  external_declaration                  /*nothing*/
	| translation_unit external_declaration /*nothing*/
	;

external_declaration:
	  function_definition   /*nothing*/
	| declaration           /*nothing*/
	;

function_definition:
	  declaration_specifiers declarator declaration_list {yyerror("not support this type of function definition");} compound_statement 
	| declaration_specifiers declarator {
            if ($1.type==NULL) yyerror("declaration error");
            if ($1.hasTYPEDEF) yyerror("funtion typedef");
            const_Typename_ptr tmptype = $1.type;
            if ($1.hasCONST)
            {
                Typename_t *tmp = memDup($1.type);
                tmp->isConst = 1;
                tmptype = tmp;
            }
            Identifier_t *id = StackDeclare(makeType(tmptype, $2), 0, 0, getDeclaratorName(&$2));
            if (id->type->type != idt_fpointer || ($2.dd->type != 5 && $2.dd->type != 6))
                yyerror("function declaration error");
            gen_func(atoi(id->TACname + 1));
            if ($2.dd->type == 5)
                $<statement_i>$.sblst = $2.dd->data.d5.pl->idList;
            else
                $<statement_i>$.sblst = NULL;
            $<statement_i>$.has_begin=0;
            $<statement_i>$.has_end=0;
        } compound_statement    {
            CounterLeaveFunc();
        }
	;

declaration_list: /* this is not supported */
	  declaration
	| declaration_list declaration
	;
%%

void yyerror(const char *s)
{
    puts(s);
    exit(1);
}
