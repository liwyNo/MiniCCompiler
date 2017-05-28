#include "typedef.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <stack>
#include <cctype>
using namespace std;
extern vector<ins> com_ins;
extern map<string, unsigned> label_table;
vector<int> global_ins;
map<string, int> gsymbol;
map<string, int> farg;

const char * param[] = {"p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9"};
#define getI(x) ((isdigit(x[0]) || x[0] == '-') ? stoi(x) : getValue(x, symbol))
#define setValue(x, y) symbol[(x)] = (y)

int getValue(string x, map<string, int> & symbol){
	if(symbol.find(x) != symbol.end())
		return symbol[x];
	if(gsymbol.find(x) != gsymbol.end())
		return gsymbol[x];
	cout << "Error! Can't find the symbol." << endl;
	return -1;
}

int exe_op2(int src1, int src2, string & op){
	switch(op.size()){
		case 1:
		switch(op[0]){
			case '+':
				return src1 + src2;
			case '-':
				return src1 - src2;
			case '*':
				return src1 * src2;
			case '/':
				return src1 / src2;
			case '%':
				return src1 % src2;
			case '<':
				return src1 < src2;
			case '>':
				return src1 > src2;
			default:
				;
				//Error
		}
		case 2:
		switch(op[0]){
			case '|':
				return src1 || src2;
			case '&':
				return src1	&& src2;
			case '!':
				return src1 != src2;
			case '=':
				return src1 == src2;
			default:
				;
				//Error
		}
		default:
			;
			//Error
	}
	return -1;
}
int exe_op1(int src1, string & op){
	if(op.size() == 1){
		switch(op[0]){
			case '-':
				return -src1;
			case '!':
				return !src1;
			default:
				;
				//Error
		}
	}
	//Error
	return -1;
}

int callFunction(unsigned long pc, unsigned long ori_pc, unsigned long arg_size, stack<int> & arg){
	map<string, int> symbol;
	stack<int> arg_s;

	//Initial argument
	for(int i = arg_size; i > 0; i--){
		int tmp = arg.top();
		arg.pop();
		symbol[param[i - 1]] = tmp;
	}

	void debugFunc(unsigned long pc, map<string, int> & lo_symbol);
	while(1){
        cout << "PC: " << pc << endl;
		ins t = com_ins[pc - 1];
		if(t.type != iNOOP)
			debugFunc(pc, symbol);
		pc += 1;
		int src1, src2, src3, des;
		switch(t.type){
			case iNOOP:
				break;
			case iOP2:
				src1 = getI(t.arg2);
				src2 = getI(t.arg4);
				des = exe_op2(src1 ,src2, t.arg3);
				setValue(t.arg1, des);
				break;
			case iOP1:
				src1 = getI(t.arg3);
				des = exe_op1(src1, t.arg2);
				setValue(t.arg1, des);
				break;
			case iASS:
				des = getI(t.arg2);
				setValue(t.arg1, des);
				break;
			case iARRSET:
				src1 = getI(t.arg1);
				src2 = getI(t.arg2);
				des = getI(t.arg3);
				src1 += src2;
				*((int*)src1) = des;
				break;
			case iARRGET:
				src1 = getI(t.arg2);
				src2 = getI(t.arg3);
				src1 += src2;
				des = *((int*)src1);
				setValue(t.arg1, des);
				break;
			case iIF:
				src1 = getI(t.arg1);
				src2 = getI(t.arg3);
				if(exe_op2(src1, src2, t.arg2))
					pc = label_table[t.arg4];
				break;
			case iGOTO:
				pc = getI(t.arg1);
				break;
			case iPARAM:
				arg_s.push(getI(t.arg1));
				break;
			case iCALLVOID:
				callFunction(label_table[t.arg1], pc, farg[t.arg1], arg_s);
				break;
			case iCALL:
				des = callFunction(label_table[t.arg2], pc, farg[t.arg2], arg_s);
				setValue(t.arg1, des);
				break;
			case iRETURN:
				if(t.arg1.size() == 0)
					return 0;
				else
					return getI(t.arg1);
				break;
			case iVAR:
				if(t.arg2.size() == 0){
					symbol[t.arg1] = 0;
				}
				else{
					symbol[t.arg1] = stoi(t.arg2);
				}
				break;
			default:
				;
				//Error
		}
	}
}

int beginProgram(unsigned long pc){
	stack<int> arg;
	auto & symbol = gsymbol;
	unsigned long tmp = 0;
    for(int i = 0; i < com_ins.size(); ++i){
        cout << "i: " << i << " INS: " << com_ins[i].type << endl;
    }
	for(auto x : global_ins){
        cout << "GL: " << x << endl;
		ins t = com_ins[x - 1];
		int src1, src2, src3, des;
		switch(t.type){
			case iNOOP:
				break;
			case iOP2:
				src1 = getI(t.arg2);
				src2 = getI(t.arg4);
				des = exe_op2(src1 ,src2, t.arg3);
				setValue(t.arg1, des);
				break;
			case iOP1:
				src1 = getI(t.arg3);
				des = exe_op1(src1, t.arg2);
				setValue(t.arg1, des);
				break;
			case iASS:
				des = getI(t.arg2);
				setValue(t.arg1, des);
				break;
			case iARRSET:
				src1 = getI(t.arg1);
				src2 = getI(t.arg2);
				des = getI(t.arg3);
				src1 += src2;
				*((int*)src1) = des;
				break;
			case iARRGET:
				src1 = getI(t.arg2);
				src2 = getI(t.arg3);
				src1 += src2;
				des = *((int*)src1);
				setValue(t.arg1, des);
				break;
			case iVAR:
				if(t.arg2.size() == 0){
					symbol[t.arg1] = 0;
				}
				else{
					symbol[t.arg1] = stoi(t.arg2);
				}
				break;
			default:
				;
			}
	}
	return callFunction(pc, -1, 0, arg);
}
