#include "typedef.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
using namespace std;

int yyparse (void);
extern FILE * yyin;

void help(const char * path) {
    cout << "Usage: " << path << " [-d] <filename>" << endl;
}

vector<string> ori_ins;
vector<ins> com_ins;
map<string, unsigned> label_table;

unsigned long pc = 1;
int main(int argc, char** argv){
	bool debugMode = false;

	ifstream is;
    //Analyze the arguments
    if(argc == 2){
        is.open(argv[1]);
        yyin = fopen(argv[1], "r");
    }
    else if(argc == 3 && argv[1] == string("-d")){
        debugMode = true;
        is.open(argv[2]);
        yyin = fopen(argv[2], "r");
    }
    else{
        help(argv[0]);
        exit(-1);
    }

    //Get the original code
    while (!is.eof()) {
        string tmp;
        getline(is, tmp);
        ori_ins.push_back(tmp);
    }

	//Compile the instruction
	com_ins.resize(ori_ins.size());
	yyparse();

    cout << "DEBUG" << endl;
    for(auto x : label_table){
        cout << x.first << " " << x.second << endl;
    }
	pc = (unsigned long)label_table["f_main"];

	void initialDebug(bool);
	initialDebug(debugMode);

	int beginProgram(unsigned long pc);
	return beginProgram(pc);
}
