%{
#include <cstdio>
#include "typedef.h"
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include "util.h"
#include <iostream>
//#include "Eeyore.tab.hpp"
using namespace std;
#define debug(x) cerr<<#x<<"="<<x<<endl

int yylex(void);
extern int yylineno;
void yyerror(const char*);

bool isGlobal = true;
Function *now_fun;
extern vector<ins> com_ins;
extern map<string, unsigned> label_table;

//extern vector<int> global_ins;
inline void checkGlobal();
%}

%code requires {
#include "typedef.h"
}

%union {
    char *vstr;
	//std::string vstring;
	RightValue_s_t RightValue_s;
    int vint;
}

%token END 0
%token EOL
%token VAR
%token IF
%token GOTO
%token PARAM
%token CALL
%token RETURN
%token ENDT
%token <vstr>NUM
%token <vstr>SYMBOL
%token <vstr>LABEL
%token <vstr>FUNCTION
%token <vstr>OP
%token <vstr>LOGIOP

%type <RightValue_s> RightValue
%type <vstr>OP2
%type <vstr> FunctionName
%type <vstr> FunctionEnd

%%
Program
:	/* Empty */
	|	Program Statement FunctionDecl 
	|	Program FunctionDecl 
	|	Program Statement END
;
FunctionDecl
	:	FunctionName Statement FunctionEnd
		{
			//已重写
			if(strcmp($1,$3)!=0)
				yyerror("FunctionName should equal to FunctionEnd!");
		}
;

FunctionName
	:	FUNCTION '[' NUM ']' EOL	{
		//已重写
		isGlobal = false;
		com_ins[yylineno - 1] = ins(iFBEGIN, $1);
		$$ = $1;
		now_fun = new_Function($1, stoi(string($3)));
	}
;
FunctionEnd
	:	ENDT FUNCTION EOL 	{
		//已重写
		isGlobal = true;
		now_fun = nullptr;
		com_ins[yylineno - 1] = ins(iFEND);
		$$ = $2;

	}
;
Statement
	:	Expression
	|	Statement Expression 
;

RightValue
	:	SYMBOL	{
		//已重写
		$$.str_name = $1; $$.Num_or_Symbol = 1;}
	|	NUM	{
		//已重写
		$$.str_name = $1; $$.Num_or_Symbol = 0; $$.real_num = stoi(string($1));}
;

OP2
	:	OP	{
		//已重写
		$$ = $1;}
	|	LOGIOP	{
		//已重写
		$$ = $1;}
;

Expression
	:	SYMBOL '=' RightValue OP2 RightValue EOL  {
			//已修改
			checkGlobal();
			string str_a = fix_name($1, now_fun), str_b = fix_name($3.str_name, now_fun), str_c = fix_name($5.str_name, now_fun);
			Variable *a = get_Var_in_Func(str_a, now_fun);
			checkNotArray(a);
			bitset<1000> def, use;
			def[a->num]=1;
			if($3.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_b, now_fun) -> num] = 1;
			if($5.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_c, now_fun) -> num] = 1;
			com_ins[yylineno - 1] = ins(iOP2, str_a, str_b, $4, str_c);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	SYMBOL '=' OP RightValue EOL  {
			//已修改
			checkGlobal();
			string str_a = fix_name($1, now_fun), str_b = fix_name($4.str_name, now_fun);
			Variable *a = get_Var_in_Func(str_a, now_fun);
			checkNotArray(a);
			bitset<1000> def, use;
			def[a->num]=1;
			if($4.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_b, now_fun) -> num] = 1;
			com_ins[yylineno - 1] = ins(iOP1, str_a, $3, str_b);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	SYMBOL '=' RightValue EOL  {
			//已修改
			//debug($1);
			//debug($3.str_name);
			//checkGlobal(); 有可能在外面有赋值
			string str_a = fix_name($1, now_fun), str_b = fix_name($3.str_name, now_fun);
			//debug(str_b);
			Variable *a = get_Var_in_Func(str_a, now_fun);
			checkNotArray(a);
			bitset<1000> def, use;
			def[a->num]=1;
			if($3.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_b, now_fun) -> num] = 1;
			//debug($3.real_num);
			//debug(yylineno - 1);
			com_ins[yylineno - 1] = ins(iASS, str_a, str_b);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	SYMBOL '[' RightValue ']' '=' RightValue EOL  {
			//已修改
			checkGlobal();
			string str_a = fix_name($1, now_fun), str_b = fix_name($3.str_name, now_fun), str_c = fix_name($6.str_name, now_fun);
			Variable *a = get_Var_in_Func(str_a, now_fun);
			checkArray(a);
			bitset<1000> def, use;
			use[a -> num] = 1;
			if($3.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_b, now_fun) -> num] = 1;
			if($6.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_c, now_fun) -> num] = 1;
			com_ins[yylineno - 1] = ins(iARRSET, str_a, str_b, str_c);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	SYMBOL '=' SYMBOL '[' RightValue ']' EOL  {
			//已修改
			checkGlobal();
			string str_a = fix_name($1, now_fun), str_b = fix_name($3, now_fun), str_c = fix_name($5.str_name, now_fun);
			Variable *a = get_Var_in_Func(str_a, now_fun), *b = get_Var_in_Func(str_b, now_fun);
			checkArray(b);
			bitset<1000> def, use;
			def[a -> num] = 1;
			use[b -> num] = 1;
			if($5.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_c, now_fun) -> num] = 1;
			com_ins[yylineno - 1] = ins(iARRGET, str_a, str_b, str_c);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	IF RightValue LOGIOP RightValue GOTO LABEL EOL {
			//已修改
			checkGlobal();
			string str_a = fix_name($2, now_fun), str_b = fix_name($4, now_fun);
			bitset<1000> def, use;
			if($2.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_a, now_fun) -> num] = 1;
			if($4.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_b, now_fun) -> num] = 1;
			com_ins[yylineno - 1] = ins(iIF, str_a, $3, str_b, $6);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}  
	|	GOTO LABEL EOL  {
			//已修改
			checkGlobal();
			com_ins[yylineno - 1] = ins(iGOTO, $2);
		}
	|	LABEL ':' EOL  {
			//已修改
			checkGlobal();
			com_ins[yylineno - 1] = ins(iLABEL, $1);
			label_table[$1] = yylineno - 1;
		}
	|	PARAM SYMBOL EOL {
			//已修改
			checkGlobal();
			string str_a = fix_name($2, now_fun);
			Variable *a = get_Var_in_Func(str_a, now_fun);
			bitset<1000> def, use;
			use[a -> num] = 1;
			com_ins[yylineno - 1] = ins(iPARAM, $2);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}  
	|	CALL FUNCTION EOL {
			//已重写
			checkGlobal();
			com_ins[yylineno - 1] = ins(iCALLVOID, $2);
		} 
	|	SYMBOL '=' CALL FUNCTION EOL {
			//已重写
			checkGlobal();
			string str_a = fix_name($1, now_fun);
			Variable *a = get_Var_in_Func(str_a, now_fun);
			bitset<1000> def, use;
			def[a -> num] = 1;
			com_ins[yylineno - 1] = ins(iCALL, $1, $4);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	RETURN RightValue EOL  {
			//已重写
			checkGlobal();
			string str_a = fix_name($2.str_name, now_fun);
			bitset<1000> def, use;
			if($2.Num_or_Symbol == 1)
				use[get_Var_in_Func(str_a, now_fun) -> num] = 1;
			com_ins[yylineno - 1] = ins(iRETURN, $2);
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	RETURN EOL {
			//已重写
			checkGlobal();
			com_ins[yylineno - 1] = ins(iNOOP);
		}
	|	VAR SYMBOL EOL{
			//已重写
			if($2[2]=='p')
				yyerror("pxx can not be declare!");
			Variable *a = new_Var($2,isGlobal, now_fun);
			if(isGlobal)
				com_ins[yylineno - 1] = ins(iGVAR, $2);
			else
				com_ins[yylineno - 1] = ins(iVAR, $2);
			//debug(a->num);
			bitset<1000> def, use;
			def[a->num] = 1;
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
			//debug(yylineno);
		}
	|	VAR NUM SYMBOL EOL{
			//已重写
			if($2[2]=='p')
				yyerror("pxx can not be declare!");
			Variable *a = new_Var_Arr($2,isGlobal, stoi(string($2)), now_fun);
			if(isGlobal)
				com_ins[yylineno - 1] = ins(iGVAR, $3, $2);
			else
				com_ins[yylineno - 1] = ins(iVAR, $3, $2);
			bitset<1000> def, use;
			def[a->num] = 1;
			com_ins[yylineno - 1].def = def;
			com_ins[yylineno - 1].use = use;
		}
	|	EOL	{
			//已重写
			com_ins[yylineno - 1] = ins(iNOOP);
		}
;
%%
void yyerror(const char *s) {
	printf("%s\n", s);
}
inline void checkGlobal() //某些语句必须在函数中
{
	if (isGlobal)
		yyerror("This expression can not be outside of a function!");
}

/*
inline void checkGlobal(unsigned long i){
	;
	//if(isGlobal)
		//global_ins.push_back(i);
}
*/
