#include "sim.h"
#include "op.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

// l1, l2, l3...
regex label_regex(R"(^\s*([l]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*):\s*(?:\/\/.*)?$)");
//function
regex function_regex(R"(^\s*(f_[a-zA-Z_][a-zA-Z_0-9]*):\s*(?:\/\/.*)?$)");
// var/gvar <type> [length] <name>
regex dec_regex(
    R"(^\s*g?var\s+(u?int[1248]|float[48]|ptr)\s+(?:([1-9]\d*)\s+)?([Ttp]\d+)\s*(?:\/\/.*)?$)");
// const <type> <name>(c1, c2...) <val>
regex const_dec_regex(
    R"(^\s*const\s+(u?int[1248]|float[48]|ptr|str)\s+(c\d+)\s+([ 0-9.]*)\s*(?:\/\/.*)?$)");
// comment
regex comment_regex(R"(^\s*\/\/.*$)");
// t1 = t1 <op> t2
regex exp1_regex(
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+)\s*([+*\/%^|&<>]|-|<<|>>|\|\||&&|[!=<>]=)\s*([Ttpc]\d+)\s*(?:\/\/.*)?$)");
// t1 = <op> t2
regex exp2_regex(
    R"(^\s*([Ttp]\d+)\s*=\s*([*&!~]|-)\s*([Ttpc]\d+)\s*(?:\/\/.*)?$)");
// t1 = <type> t2
regex exp3_regex(
    R"(^\s*([Ttp]\d+)\s*=\s*(u?int[1248]|float[48]|ptr)\s+([Ttpc]\d+)\s*(?:\/\/.*)?$)");
// t1 = t2
regex exp4_regex(
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*)\s*(?:\/\/.*)?$)");
// t1[t2]=t3
regex exp5_regex(
    R"(^\s*([Ttpc]\d+)\s*\[\s*([Ttpc]\d+|\d+)\s*\]\s*=\s*([Ttpc]\d+)\s*(?:\/\/.*)?$)");
//*t1=t2
regex exp6_regex(
    R"(^\s*\*\s*([Ttpc]\d+)\s*=\s*([Ttpc]\d+)\s*(?:\/\/.*)?$)");
// t1=t2[t3]
regex exp7_regex(
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+)\s*\[\s*([Ttpc]\d+|\d*)\s*\]\s*(?:\/\/.*)?$)");
// goto l1
regex goto_regex(
    R"(^\s*goto\s+([l]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*)\s*(?:\/\/.*)?$)");
// if t1 <op> t2 goto l1
regex if_regex(
    R"(^\s*if\s+([Ttpc]\d+)\s*([<>]|[!<>=]=)\s*([Ttpc]\d+)\s+goto\s+([l]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*)\s*(?:\/\/.*)?$)");
// if t1 <op> t2 goto l1
regex if2_regex(
    R"(^\s*if\s+([Ttpc]\d+)\s+goto\s+([l]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*)\s*(?:\/\/.*)?$)");
// param t1
regex param_regex(
    R"(^\s*param\s+([Ttpc]\d+|f_[a-zA-Z_][a-zA-Z0-9_]*)\s*(?:\/\/.*)?$)");
// call f 3
regex call_regex(
    R"(^\s*call\s+([lTcpt]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*)\s+(\d+)\s*(?:\/\/.*)?$)");
// t1 = call f 3
regex call2_regex(
    R"(^\s*([tTp]\d+)\s*=\s*call\s+([lTcpt]\d+|f_[a-zA-Z_][a-zA-Z_0-9]*)\s+(\d+)\s*(?:\/\/.*)?$)");
// return [t1]
regex return_regex(R"(^\s*return(?:\s+([Ttcp]\d+))?\s*(?:\/\/.*)?$)");

regex end_regex(R"(^\s*end\s+(f_[a-zA-Z_][_0-9a-zA-Z]*)\s*(?:\/\/.*)?$)");

// Next instruction
size_t pc;
// To store all instruction
vector<string> ins;
// Map the string type to TypeName
unordered_map<string, TypeName> type_table;
// Map the symbol type to value and Var
unordered_map<string, Var> symbol_table;
//
unordered_map<string, int> funcMap;
// Store compiled instruction
vector<tuple<int, string, string, string, string>> c_ins;

// get the Var of symbol src1
TypeName getType(string src1) { return symbol_table[src1].type; }

string toString(string src) {
    Var t = symbol_table[src];
    switch (t.type) {
    case TypeName::Int1:
        return to_string(t.value.int1);
    case TypeName::Int2:
        return to_string(t.value.int2);
    case TypeName::Int4:
        return to_string(t.value.int4);
    case TypeName::Int8:
        return to_string(t.value.int8);
    case TypeName::Uint1:
        return to_string(t.value.uint1);
    case TypeName::Uint2:
        return to_string(t.value.uint2);
    case TypeName::Uint4:
        return to_string(t.value.uint4);
    case TypeName::Uint8:
        return to_string(t.value.uint8);
    case TypeName::Float:
        return to_string(t.value.f);
    case TypeName::Double:
        return to_string(t.value.d);
    case TypeName::Pointer:
        return to_string((unsigned long long)t.value.ptr);
    default:
        return "";
    }
}
void setVal(string &s1, TypeName t1, string val) {
    symbol_table[s1].value.uint8 = 0;
    switch (t1) {
    case TypeName::Int1:
        symbol_table[s1].value.int1 = stoi(val);
        break;
    case TypeName::Int2:
        symbol_table[s1].value.int2 = stoi(val);
        break;
    case TypeName::Int4:
        symbol_table[s1].value.int4 = stoi(val);
        break;
    case TypeName::Int8:
        symbol_table[s1].value.int8 = stoll(val);
        break;
    case TypeName::Uint1:
        symbol_table[s1].value.uint1 = stoul(val);
        break;
    case TypeName::Uint2:
        symbol_table[s1].value.uint2 = stoul(val);
        break;
    case TypeName::Uint4:
        symbol_table[s1].value.uint4 = stoul(val);
        break;
    case TypeName::Uint8:
        symbol_table[s1].value.uint8 = stoull(val);
        break;
    case TypeName::Float:
        symbol_table[s1].value.f = stof(val);
        break;
    case TypeName::Double:
        symbol_table[s1].value.d = stod(val);
        break;
    case TypeName::Pointer:
    case TypeName::String:
        symbol_table[s1].value.ptr = (void *)stoull(val);
        break;
    }
}

void printVar(ostream &os, string &v) {
    if(v == "pc"){
        os << "pc: "<< pc << endl;
        return;
    }
    if(symbol_table.find(v) == symbol_table.end()){
        os << "No symbol: " << v << endl;
        return;
    }
    os << v << " " << type_cstr[int(symbol_table[v].type)] << " "
       << symbol_table[v].length << " " << toString(v) << endl;
}

void build_label_table() {
    // Initialize the label table
    smatch match_result;
    int lineCounter = 0;
    for (auto x : ins) {
        lineCounter++;
        if (regex_match(x, match_result, label_regex)) {
            string match_str = match_result[1].str();
            if (symbol_table.find(match_str) == symbol_table.end()) {
                Var t;
                t.type = TypeName::Pointer;
                t.value.uint8 = lineCounter;
                symbol_table[match_str] = t; // Label point to next line
            } else {
                cerr << "Redecleration of label or funtion (Line "
                     << lineCounter << "): " << match_str << endl;
                exit(-1);
            }
        }
    }

#ifdef DEBUG
    // Print the label table
    cout << "\tLabel Table" << endl;
    cout << "label\tline\tvalue" << endl;
    for (auto x : symbol_table)
        cout << x.first << "\t" << x.second.value.uint8 << "\t"
             << toString(x.first) << endl;
#endif
}
void buildInFuncList(){
    funcMap["f_getchar"] = 0;
    funcMap["f_putchar"] = 1;
    funcMap["f_getint"] = 0;
    funcMap["f_putint"] = 1;
}
bool buildInFunc(string func){
    return funcMap.find(func) != funcMap.end();
}
bool insert_symbol_table(string &&name_str, string &&type_str,
                         string &&length_str, string &&value_str) {
    if (length_str == "")
        length_str = "0";
    if (name_str[0] != 'p' && symbol_table.find(name_str) != symbol_table.end())
        return false;
    symbol_table[name_str] = Var();
    Var &t = symbol_table[name_str];
    t.type = type_table[type_str];
    t.length = stoull(length_str);
    switch (t.type) {
    case TypeName::String: {
        istringstream is(value_str);
        int tmp, byteCount = 0;
        vector<int> tmpV;
        while (!is.eof()) {
            is >> tmp;
            byteCount++;
            tmpV.push_back(tmp);
        }
        t.value.ptr = new char[tmpV.size()];
        t.length = tmpV.size();
        for (int i = 0; i < tmpV.size(); ++i) {
            ((char *)t.value.ptr)[i] = tmpV[i];
            if (!tmpV[i])
                break;
        }
        t.type = TypeName::Pointer;
        break;
    }
    case TypeName::Pointer:{
        if(length_str != "0")
            t.value.ptr = new char [t.length];
        else
            setVal(name_str, t.type, value_str);
        break;
    }
    default:
        setVal(name_str, t.type, value_str);
    }
    return true;
}
void compile_ins() {
    smatch match_result;
    // check the Var of "t1 = t2 op t2"
    auto typeCheck2 = [](string &&des, string &&src1, string &&op,
                         string &&src2) {
        TypeName t1 = getType(src1), t2 = getType(src2), td = getType(des);

        // check src1 and src2 type
        if ((t1 != t2) && (t1 != TypeName::Pointer || t2 != TypeName::Int4) &&
            (t1 != TypeName::Int4 || t2 != TypeName::Pointer))
            return false;

        // Handle some op can only use between two integer
        regex intop_e1(R"([%^|&]|\|\||&&|<<|>>)");
        if (regex_match(op, intop_e1)) {
            if (t1 >= TypeName::Int1 && t1 <= TypeName::Uint8 && td == t1)
                return true;
            return false;
        }
        // logical operation return INT4
        regex intop_e2(R"(^[<>]|[<>!=]=$)");
        if (regex_match(op, intop_e2) && td == TypeName::Int4)
            return true;

        // handle other op, td must equal to t1 and t2
        return td == t1;
    };
    auto typeCheck = [](string &&des, string &&op, string &&src) {
        TypeName ts = getType(src), td = getType(des);
        switch (op[0]) {
        case '*':
            return ts == TypeName::Pointer;
        case '&':
            return td == TypeName::Pointer;
        case '-':
            return ts != TypeName::Pointer and ts == td;
        case '~':
            return ts <= TypeName::Uint8 and ts == td;
        case '!':
            return td == TypeName::Int4;
        }
        return false;
    };
    auto exist = [](string &&v) {
        return symbol_table.find(v) != symbol_table.end();
    };
    // compile the instruction
    size_t lineCounter = 0;
    string env = "";
    for (auto x : ins) {
        lineCounter++;
        int match_count = 0, match_type = 0;
        do {
            // comment / label / declaration
            if ((x == "") || regex_match(x, comment_regex) ||
                regex_match(x, label_regex)){
                if(regex_match(x, match_result, function_regex)){
                    env = match_result[1].str();
                }
                break;
            }
            // end f_xxx
            if(regex_match(x, end_regex)){
                env = "";
                break;
            }
            // gvar / var
            if (regex_match(x, match_result, dec_regex)) {
                insert_symbol_table(match_result[3].str(),
                                    match_result[1].str(),
                                    match_result[2].str(), "0");
                break;
            }
            // const
            if (regex_match(x, match_result, const_dec_regex)) {
                insert_symbol_table(match_result[2].str(),
                                    match_result[1].str(), "0",
                                    match_result[3].str());
                break;
            }
            // t1 = t2 op t3
            if (regex_match(x, match_result, exp1_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[2].str()) ||
                    !exist(match_result[4].str())) {
                    // Error
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (!typeCheck2(match_result[1].str(), match_result[2].str(),
                                match_result[3].str(), match_result[4].str())) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 4;
                match_type = 1;
                break;
            }
            // t1 = op t2
            if (regex_match(x, match_result, exp2_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[3].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (!typeCheck(match_result[1].str(), match_result[2].str(),
                              match_result[3].str())) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 3;
                match_type = 2;
                break;
            }
            // t1 = <type> t2
            if (regex_match(x, match_result, exp3_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[3].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (type_table[match_result[2].str()] !=
                    getType(match_result[1].str())) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 3;
                match_type = 3;
                break;
            }
            // t1 = t2
            if (regex_match(x, match_result, exp4_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[2].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (getType(match_result[2].str()) !=
                    getType(match_result[1].str())) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 2;
                match_type = 4;
                break;
            }
            // t1 [t2] = t3
            if (regex_match(x, match_result, exp5_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[2].str()) ||
                    !exist(match_result[3].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (getType(match_result[2].str()) > TypeName::Uint8 ||
                    getType(match_result[1].str()) != TypeName::Pointer) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 3;
                match_type = 5;
                break;
            }
            // * t1 = t2
            if (regex_match(x, match_result, exp6_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[2].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (getType(match_result[1].str()) != TypeName::Pointer) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 2;
                match_type = 6;
                break;
            }
            // t1 = t2[t3]
            if (regex_match(x, match_result, exp7_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[2].str()) ||
                    !exist(match_result[3].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (getType(match_result[3].str()) > TypeName::Uint8 ||
                    getType(match_result[2].str()) != TypeName::Pointer) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 3;
                match_type = 7;
                break;
            }
            // goto l1
            if (regex_match(x, match_result, goto_regex)) {
                if (!exist(match_result[1].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 1;
                match_type = 8;
                break;
            }
            // if t1 op t2 goto l1
            if (regex_match(x, match_result, if_regex)) {
                if (!exist(match_result[1].str()) ||
                    !exist(match_result[3].str()) ||
                    !exist(match_result[4].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                if (getType(match_result[1].str()) !=
                    getType(match_result[3].str())) {
                    cerr << "Type check fail (Line" << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 4;
                match_type = 9;
                break;
            }
            // param t1
            if (regex_match(x, match_result, param_regex)) {
                if (!exist(match_result[1].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 1;
                match_type = 10;
                break;
            }
            // call f 3
            if (regex_match(x, match_result, call_regex)) {
                if (!exist(match_result[1].str()) &&
                    !buildInFunc(match_result[1].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 2;
                match_type = 11;
                break;
            }
            // return [t1]
            if (regex_match(x, match_result, return_regex)) {
                if (match_result[1].str() != "" &&
                    !exist(match_result[1].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 1;
                match_type = 12;
                break;
            }
            // t1 = call f 3
            if (regex_match(x, match_result, call2_regex)) {
                if (!exist(match_result[1].str()) ||
                    (!exist(match_result[2].str()) &&
                     !buildInFunc(match_result[2].str()))) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 3;
                match_type = 13;
                break;
            }
            if (regex_match(x, match_result, if2_regex)) {
                if (!exist(match_result[1].str()) || !exist(match_result[2].str())) {
                    cerr << "Undefined symbol (Line " << lineCounter
                         << "): " << x;
                    exit(-1);
                }
                match_count = 2;
                match_type = 14;
                break;
            }
            // Error no Var match!
            cerr << "Ins (Line " << lineCounter << "):\"" << x
                 << "\" can't compile." << endl;
            exit(-1);
        } while (0);
        string match_str[4];
        switch (match_count) {
        case 4:
            match_str[3] = match_result[4].str();
        case 3:
            match_str[2] = match_result[3].str();
        case 2:
            match_str[1] = match_result[2].str();
        case 1:
            match_str[0] = match_result[1].str();
        case 0:
            c_ins.emplace_back(match_type, match_str[0], match_str[1],
                               match_str[2], match_str[3]);
        }
        void execute(int);
        if(env == "")
            execute(lineCounter - 1);
    }
#ifdef DEBUG
    cout << "\tInstruction table" << endl;
    for (auto x : c_ins) {
        cout << get<0>(x) << " " << get<1>(x) << " " << get<2>(x) << " "
             << get<3>(x) << " " << get<4>(x) << " " << endl;
    }
#endif
}
bool initialize(ifstream &in) {
    for (int i = 0; i < 11; ++i) {
        type_table[type_cstr[i]] = TypeName(i);
    }
    type_table["str"] = TypeName::String;
    // Read from input file
    while (!in.eof()) {
        string tmp;
        getline(in, tmp);
        ins.push_back(tmp);
    }

    build_label_table();

    buildInFuncList();

    compile_ins();

    symbol_table["tmp"] = Var();
    return true;
}

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

stack<Var> argStack;
stack<Var> backupStack;
stack<size_t> argSizeStack;
stack<size_t> pcStack;
stack<string> rValStack;

bool debugMode = false;
void help(const char * path) {
    cout << "Usage: " << path << " [-d] <filename>" << endl;
}

void executeBuildinFunc(string func, string des){
    if(func == "f_getchar"){
        char tmp = cin.get();
        setVal(des, TypeName::Int1, to_string(tmp));
        return;
    }
    if(func == "f_putchar"){
        Var t = argStack.top();
        argStack.pop();
        cout.put(t.value.int1);
        return;
    }
    if(func == "f_getint"){
        int tmp;
        cin >> tmp;
        setVal(des, TypeName::Int4, to_string(tmp));
        return;
    }
    if(func == "f_putint"){
        Var t = argStack.top();
        argStack.pop();
        cout << t.value.int4;
        return;
    }
}

void call_exe(string func, size_t argc){
    pcStack.push(pc);
    for(int i = argSizeStack.top(); i > 0; i--){
        backupStack.push(symbol_table[string("p") + to_string(i - 1)]);
    }
    argSizeStack.push(argc);
    pc = (unsigned long long)symbol_table[func].value.ptr;
    for (size_t i = argc; i > 0; --i) {
        string var_name = string("p") + to_string(i - 1);
        symbol_table[var_name] = argStack.top();
        argStack.pop();
    }
}

void execute(int pc_l) {
#ifdef DEBUG
    cout << "pc_l: " << pc_l << endl;
#endif
    auto t_ins = c_ins[pc_l];
    pc++;
    switch (get<0>(t_ins)) {
    case 0:
        return;
    case 1:
        s_op2(get<1>(t_ins), get<2>(t_ins), get<3>(t_ins), get<4>(t_ins));
        return;
    case 2:
        s_op1(get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
        return;
    case 3:
        s_cast(get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
        return;
    case 4:
        s_cpy(get<1>(t_ins), get<2>(t_ins));
        return;
    case 5:
        s_arr_cpy(get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
        return;
    case 6:
        s_pnt_cpy(get<1>(t_ins), get<2>(t_ins));
        return;
    case 7:
        s_cpy_arr(get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
        return;
    case 8:
        // goto l1
        goto_exe(get<1>(t_ins), pc);
        return;
    case 9: {
        string tmp("tmp");
        symbol_table[tmp].value.int4 = 0;
        s_op2(tmp, get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
        if (symbol_table["tmp"].value.int4) {
            pc = (unsigned long long)symbol_table[get<4>(t_ins)].value.ptr;
        }
        return;
    }
    case 10:
        // param t1
        argStack.push(symbol_table[get<1>(t_ins)]);
        return;
    case 11:
        // call f1 3
        if(buildInFunc(get<1>(t_ins))){
            if(stoi(get<2>(t_ins)) != funcMap[get<1>(t_ins)]){
                cerr << "BuildinFunc call fail -- wrong size of arguments" << endl;
                exit(-1);
            }
            executeBuildinFunc(get<1>(t_ins), "tmp");
            return;
        }
        call_exe(get<1>(t_ins), stoull(get<2>(t_ins)));
        return;
    case 12:
        // return
        pc = pcStack.top();
        // pc == -1 mean that the program has exit the main function.
        if(pc == (unsigned long long) -1)
            return;
        pcStack.pop();
        if (get<1>(t_ins) != "") {
            string r_val = rValStack.top();
            rValStack.pop();
            s_cpy(r_val, get<1>(t_ins));
        }
        argSizeStack.pop();
        for(int i = 0; !argSizeStack.empty() && i < argSizeStack.top(); i++){
            symbol_table[string("p") + to_string(i)] = backupStack.top();
            backupStack.pop();
        }
        return;
    case 13:
        if(buildInFunc(get<2>(t_ins))){
            if(stoi(get<3>(t_ins)) != funcMap[get<2>(t_ins)]){
                cerr << "BuildinFunc call fail -- wrong size of arguments" << endl;
                exit(-1);
            }
            executeBuildinFunc(get<2>(t_ins), get<1>(t_ins));
            return;
        }
        rValStack.push(get<1>(t_ins));
        call_exe(get<2>(t_ins), stoull(get<3>(t_ins)));
        return;
    case 14:
        if(symbol_table[get<1>(t_ins)].value.uint8)
            goto_exe(get<2>(t_ins), pc);
        return;
    }
}
static int debugMod = 1;
static size_t debugCount = 0;
void intoDebug(){
    string tmp;
    while(1){
        cout << "PC: " << pc << " " << ins[pc] << endl;
        cout << "> ";
        cin >> tmp;
        if(tmp == "p"){
            // Print symbol
            cin >> tmp;
            printVar(cout, tmp);
            continue;
        }
        if(tmp == "s"){
            // Step in
            int nStep = 0;
            cin >> nStep;
            debugMod = 1;
            debugCount = nStep;
            return;
        }
        if(tmp == "n"){
            //next step
            debugMod = 1;
            debugCount = 1;
            return;
        }
        if(tmp == "u"){
            // Until
            cin >> tmp;
            debugMod = 2;
            if(tmp[0] == 'f' || tmp[0] == 'l'){
                debugCount = symbol_table[tmp].value.uint8;
            }
            else{
                debugCount = stoull(tmp);
            }
            return;
        }
        if(tmp == "r"){
            //Run
            debugMode = false;
            return;
        }
    }
}
void debugFunc(){
    //debugMod: 1. run n step
    //          2. run until pc equal debugCount
    if(debugMod == 1){
        if(debugCount)
            debugCount --;
        else{
            intoDebug();
            if(debugMod == 1)
                debugCount--;
            return;
        }
    }
    else if(debugMod == 2){
        if(debugCount == pc){
            intoDebug();
        }
    }
}
int main(int argc, char **argv) {
    ifstream is;
    if(argc == 2){
        is.open(argv[1]);
    }
    else if(argc == 3 && argv[1] == string("-d")){
        debugMode = true;
        is.open(argv[2]);
    }
    else{
        help(argv[0]);
        exit(-1);
    }
    initialize(is);


    pc = (unsigned long long)symbol_table["f_main"].value.ptr;
    pcStack.push(-1);
    argSizeStack.push(0);
#ifdef DEBUG
    cout << "BEGIN PROGRAM" << endl;
#endif
    while (pc != (unsigned long long)-1) {
        if(debugMode)
            debugFunc();
        execute(pc);
    }
#ifdef DEBUG
    cout << "EXIT NORMALLY" << endl;
#endif
    return 0;
}
