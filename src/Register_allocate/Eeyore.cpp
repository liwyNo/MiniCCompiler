#include "typedef.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include "Eeyore.tab.hpp"
#include "analysis.h"
#include "allocate.h"
#include "util.h"

using namespace std;
#define debug(x) cerr<<#x<<"="<<x<<endl

int yyparse (void);
extern FILE * yyin;

void help(const char * path) {
    cout << "Usage: " << path << " [-d] <filename>" << endl;
}

vector<string> ori_ins;
vector<ins> com_ins;
map<string, unsigned> label_table;


int main(int argc, char** argv){
//这些代码都是直接抄的 simulator 的
	ifstream is;
    //Analyze the arguments
    if(argc == 2){
        is.open(argv[1]);
        yyin = fopen(argv[1], "r");
    }
    else{
        help(argv[0]);
        exit(-1);
    }

    //Get the original code
    ori_ins.push_back("");//把下表零空过去
    while (!is.eof()) {
        string tmp;
        getline(is, tmp);
        ori_ins.push_back(tmp);
        //debug(tmp);
    }

	//Compile the instruction
	com_ins.resize(ori_ins.size());
    //debug(123);
	yyparse();
    for (int i=1;i<com_ins.size();i++)
        com_ins[i].line_num = i;

    //init_debug();
    /*data flow analysis*/
    init_preI();
    //init_debug();
    LiveVariableAnalysis();
    //init_debug();

    /*Register Allocate:linear scan!*/
    init_all_reg();
    //init_debug();
    //return 0;
    LinearScan();
    //init_debug();
    gen_output();
	return 0;
}