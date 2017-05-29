#ifndef TYPEDEF_H
#define TYPEDEF_H

#define iOP2	1
#define iOP1	2
#define iASS	3
#define iARRSET	4
#define iARRGET	5
#define iIF	6
#define iGOTO	7
#define iPARAM	8
#define iCALLVOID	9
#define iCALL 	10
#define iRETURN	11
#define iVAR	12
#define iGVAR 	13
#define iFBEGIN 14
#define iFEND	15
#define iLABEL  16
#define iNOOP	0

#include <string>
#include <vector>
#include <map>
#include <bitset>
void yyerror(char *);


struct ins{
	int type;
	std::string arg1, arg2, arg3, arg4;
	//std::string go_label; //要到达的 label
	std::bitset<1000> def, use, live; //bitset 很省时间和空间，所以开这么大也无所谓 
	std::vector<ins*> preI;
	ins() = default;
	
	ins(int _type, std::string _arg1 = "", std::string _arg2 = "", 
		std::string _arg3 = "", std::string _arg4 = ""): 
		type(_type), arg1(_arg1), arg2(_arg2), arg3(_arg3) ,arg4(_arg4){
			def = use = 0;
		}
		
};

struct RightValue_s_t
{
	char *str_name;
	int real_num;//假如是个数字，那么就存下来值
	bool Num_or_Symbol; //0 表示是数字，否则是变量
	operator std::string()
	{
		return std::string(str_name);
	}
};

struct Variable;
struct Function
{
	int arg_num;
	int stack_size;
	std::string f_name;
	std::vector<Variable *> local_var;
	Function(std::string _name, int _num);
	void Print_Func();
};
std::string extend_p_name(std::string p_name, std::string f_name);
Function* new_Function(std::string fun_name, int arg_num);
Function* get_Fun(std::string fun_name);

struct Variable
{
	int num; //全局的编号
	bool isGlobal;
	bool isArray;//array变量不能修改。。。
	int arr_size;//若是数组，则该值为数组大小
	std::string s_name;//该变量原先的名字
	std::string v_name;//假如是全局变量，就给分配个新名字
	int spill_loc;//假如是局部变量，则有一个溢出到栈的对应位置
	Function *fa_func; //假如是局部变量，则存一下它在哪个函数里面

	Variable(bool _isGlobal, std::string _s_name, bool _isArray);
	void Print_Var();
};
Variable* new_Var(std::string var_name, int isGlobal, Function* now_fun);
Variable* new_Var_Arr(std::string var_name, int isGlobal, int size, Function* now_fun);
Variable* get_Var(std::string var_name);
Variable* get_Var_in_Func(std::string var_name, Function *now_fun);

#endif