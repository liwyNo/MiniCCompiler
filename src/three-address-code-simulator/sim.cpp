#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <tuple>
using namespace std;
union UnionType {
    // int
    int8_t int1;
    int16_t int2;
    int32_t int4;
    int64_t int8;

    // unsigned int
    uint8_t uint1;
    uint16_t uint2;
    uint32_t uint4;
    uint64_t uint8;

    // float point
    float f;
    double d;

    // ptr
    void *ptr;
};
enum class TypeName {
    Int1,
    Int2,
    Int4,
    Int8,

    Uint1,
    Uint2,
    Uint4,
    Uint8,

    Float,
    Double,

    Pointer,
    String,
    Error
};
enum class InsType {
    Comment,
    Expr1,
    Expr2,
    Expr3,
    Expr4,
    Expr5,
    Expr6,
    Expr7,
    Goto,
    If,
    Param,
    Call,
    Return,
    CallEq
};
struct Type {
    TypeName name;
    UnionType value;
    size_t length;
};
const string label_regex = R"(^\s*([fl]\d)+:\s*(?:\/\/.*)?$)";
const string dec_regex =
    R"(^\s*g?var\s+(u?int[1248]|float[48]|ptr)\s+(?:([1-9]\d*)\s+)?([Ttp]\d+)\s*(?:\/\/.*)?$)";
const string const_dec_regex =
    R"(^\s*const\s+(u?int[1248]|float[48]|ptr|str)\s+(c\d+)\s+(".*"|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
const string int_regex = R"(^\d+$)";
const string float_regex = R"(^\d+\.\d+)";
const string comment_regex = R"(^\s*\/\/.*$)";

// t1 = t1 <op> t2
const string exp1_regex =
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*([+*\/%^|&<>]|-|<<|>>|\|\||&&|[!=<>]=)\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
// t1 = <op> t2
const string exp2_regex =
    R"(^\s*([Ttp]\d+)\s*=\s*([*&!~]|-)\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
// t1 = <type> t2
const string exp3_regex =
    R"(^\s*([Ttp]\d+)\s*=\s*(u?int[1248]|float[48]|ptr)\s+([Ttpc]\d+|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
// t1 = t2
const string exp4_regex =
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
// t1[t2]=t3
const string exp5_regex =
    R"(^\s*([Ttpc]\d+)\s*\[\s*([Ttpc]\d+|\d+)\s*\]\s*=\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
//*t1=t2
const string exp6_regex =
    R"(^\s*\*\s*([Ttpc]\d+)\s*=\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*(?:\/\/.*)?$)";
// t1=t2[t3]
const string exp7_regex =
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s*\[\s*([Ttpc]\d+|\d*)\s*\]\s*(?:\/\/.*)?$)";
const string goto_regex = R"(^\s*goto\s+([lf]\d+)\s*(?:\/\/.*)?$)";
const string if_regex=R"(^\s*if\s+([Ttpc]\d+|\d+(?:\.\d+)?)\s*([<>]|[!<>=]=)\s*([Ttpc]\d+|\d+(?:\.\d+)?)\s+goto\s+([lf]\d+)\s*(?:\/\/.*)?$)";
const string param_regex = R"(^\s*param\s+([lf]\d+)\s*(?:\/\/.*)?$)";
const string call_regex = R"(^\s*call\s+([lf]\d+)\s+(\d+)\s*(?:\/\/.*)?$)";
const string call2_regex =
    R"(^\s*([tTp]\d+)\s*=\s*call\s+([lf]\d+)\s+(\d+)\s*(?:\/\/.*)?$)";
const string return_regex = R"(^\s*return(?:\s+([Ttp]\d+))?\s*(?:\/\/.*)?$)";

const string type_cstr[] = {"int1",   "int2",   "int4",  "int8",
                            "uint1",  "uint2",  "uint4", "uint8",
                            "float4", "float8", "ptr",   "str"};
const int type_len[] = {1, 2, 4, 8, 1, 2, 4, 8, 4, 8, sizeof(void *)};
int pc;                                     // Next instruction
vector<string> ins;                         // To store all instruction
unordered_map<string, TypeName> type_table; // Map the string name to TypeName
unordered_map<string, int> label_table;     // Map the label name to line number
unordered_map<string, Type>
    symbol_table; // Map the symbol name to value and type
vector<tuple<int, string, string, string, string>> c_ins;
bool initialize(ifstream &in) {
    for (int i = 0; i < 12; ++i) {
        type_table[type_cstr[i]] = TypeName(i);
    }
    // Read from input file
    while (!in.eof()) {
        string tmp;
        getline(in, tmp);
        ins.push_back(tmp);
    }

    // Initialize the label table
    regex label_e(label_regex);
    smatch match_result;
    int lineCounter = 0;
    for (auto x : ins) {
        if (regex_match(x, match_result, label_e)) {
            string match_str = match_result[1].str();
            if (label_table.find(match_str) == label_table.end()) {
                label_table[match_str] =
                    lineCounter + 1; // Label point to next line
            } else {
                // Error - same label
            }
        }
        lineCounter++;
    }
#ifdef DEBUG
    // Print the label table
    cout << "\tLabel Table" << endl;
    cout << "label\tline" << endl;
    for (auto x : label_table)
        cout << x.first << "\t" << x.second << endl;
#endif

    // Initialize the symbol table
    regex symbol_e(dec_regex);
    regex const_e(const_dec_regex);
    for (auto x : ins) {
        if (regex_match(x, match_result, symbol_e)) {
            // Get the match str
            string type_str = match_result[1].str();
            string length_str = match_result[2].str();
            string name_str = match_result[3].str();

            // Analyze the type
            TypeName type_name = type_table[type_str];
            size_t length_int = length_str == ""
                                    ? /*type_len[int(type_name)]*/ 0
                                    : stoi(length_str);

            Type t;
            t.name = type_name;
            t.length = length_int;
            if (t.name == TypeName::Pointer and t.length != 0)
                t.value.ptr = new char[t.length];
            if (symbol_table.find(name_str) == symbol_table.end()) {
                symbol_table[name_str] = t;
            } else {
                // Error - same symbol
            }
        } else {
            if (regex_match(x, match_result, const_e)) {
                // Get the match str
                string type_str = match_result[1].str();
                string name_str = match_result[2].str();
                string value_str = match_result[3].str();

                // Analyze the type
                TypeName type_name = type_table[type_str];

                Type t;
                t.name = type_name;
                switch (type_name) {
                case TypeName::Int1: {
                    t.value.int1 = stoi(value_str);
                    break;
                }
                case TypeName::Int2: {
                    t.value.int2 = stoi(value_str);
                    break;
                }
                case TypeName::Int4: {
                    t.value.int4 = stoi(value_str);
                    break;
                }
                case TypeName::Int8:
                case TypeName::Pointer: {
                    t.value.int8 = stoll(value_str);
                    break;
                }
                case TypeName::Uint1:
                case TypeName::Uint2:
                case TypeName::Uint4:
                case TypeName::Uint8: {
                    t.value.uint8 = stoull(value_str);
                    break;
                }
                case TypeName::Float: {
                    t.value.f = stof(value_str);
                    break;
                }
                case TypeName::Double: {
                    t.value.d = stod(value_str);
                    break;
                }
                case TypeName::String: {
                    t.value.ptr =
                        new char[value_str.length() - 1]; //"xxx" length = 5
                    sscanf((char *)t.value.ptr, "\"%s\"", value_str.c_str());
                    t.length = value_str.length() - 2;
                    break;
                }
                }
                if (symbol_table.find(name_str) == symbol_table.end()) {
                    symbol_table[name_str] = t;
                } else {
                    // Error
                }
            }
        }
    }
#ifdef DEBUG
    // Print the symbol table
    cout << "\tSymbol Table" << endl;
    cout << "symbol\ttype\tlength" << endl;
    for (auto x : symbol_table) {
        cout << x.first << "\t" << type_cstr[int(x.second.name)] << "\t"
             << x.second.length << endl;
    }
#endif
    //Complie the instruction
    regex comment(comment_regex), expr1(exp1_regex), expr2(exp2_regex), expr3(exp3_regex), expr4(exp4_regex), expr5(exp5_regex);
    regex expr6(exp6_regex), expr7(exp7_regex), Goto(goto_regex),If(if_regex), Param(param_regex),
          Call(call_regex), Return(return_regex), CallEq(call2_regex);
    
    for(auto x : ins){
        int match_count = 0, match_type = 0;
        cout << x << endl;
        do{
            if(regex_match(x, comment) | regex_match(x, label_e) | regex_match(x, symbol_e) | regex_match(x, const_e))
                break;
            if(regex_match(x, match_result, expr1)){
                match_count = 4;
                match_type = 1;
                break;
            }
            if(regex_match(x, match_result, expr2)){
                match_count = 3;
                match_type = 2;
                break;
            }
            if(regex_match(x, match_result, expr3)){
                match_count = 3;
                match_type = 3;
                break;
            }
            if(regex_match(x, match_result, expr4)){
                match_count = 2;
                match_type = 4;
                break;
            }
            if(regex_match(x, match_result, expr5)){
                match_count = 3;
                match_type = 5;
                break;
            }
            if(regex_match(x, match_result, expr6)){
                match_count = 2;
                match_type = 6;
                break;
            }
            if(regex_match(x, match_result, expr7)){
                match_count = 3;
                match_type = 7;
                break;
            }
            if(regex_match(x, match_result, Goto)){
                match_count = 1;
                match_type = 8;
                break;
            }
            if(regex_match(x, match_result, If)){
                match_count = 4;
                match_type = 9;
                break;
            }
            if(regex_match(x, match_result, Param)){
                match_count = 1;
                match_type = 10;
                break;
            }
            if(regex_match(x, match_result, Call)){
                match_count = 2;
                match_type = 11;
                break;
            }
            if(regex_match(x, match_result, Return)){
                match_count = 1;
                match_type = 12;
                break;
            }
            if(regex_match(x, match_result, CallEq)){
                match_count = 3;
                match_type = 13;
                break;
            }

            //Error no type match!
        }while(0);
        string match_str[4]; 
        switch(match_count){
            case 4:
                match_str[3] = match_result[4].str();
            case 3:
                match_str[2] = match_result[3].str();
            case 2:
                match_str[1] = match_result[2].str();
            case 1:
                match_str[0] = match_result[1].str();
            case 0:
                c_ins.emplace_back(match_type, match_str[0], match_str[1], match_str[2], match_str[3]);
        }
    }
#ifdef DEBUG
    cout << "\tInstruction table" << endl;
    for(auto x : c_ins){
        cout << get<0>(x) << " "
            << get<1>(x) << " "
            << get<2>(x) << " "
            << get<3>(x) << " "
            << get<4>(x) << " "
            << endl;
    }
#endif 
    return true;
}
stack<Type> argStack;
stack<int> pcStack;
stack<string> rValStack;
void help() {}
bool isVar(string & s){
    return s[0] < '0' || s[0] > '9';
}
TypeName getType(string & src1){
    if(isVar(src1)){
        return symbol_table[src1].name;
    }
    else{
        if(src1.find('.')!=src1.end())
            return TypeName::Int4;
        else
            return TypeName::Double;
    }
}
TypeName typeCheck(string & src1, string & op, string & src2){
    TypeName t1 = getType(src1), t2 = getType(src2);
    // Handle ptr 
    if(t1 == TypeName::Pointer){
        if(t2 >= (int)TypeName::Int1 && t2 <= (int)TypeName::Uint8 && (op == "+" || op == "-")){
            return TypeName::Pointer;
        }
        return TypeName::Error;
    }
    if(t2 == TypeName::Pointer){
        if(t1 >= (int)TypeName::Int1 && t1 <= (int)TypeName::Uint8 && (op == "+" || op == "-")){
            return TypeName::Pointer;
        }
        return TypeName::Error;
    }
    // Handle some op can only use between two integer
    regex intop_e(R"([%^|&]|\|\||&&|<<|>>)");
    if(regex_match(op, intop_e)){
        if(t1 >= (int)TypeName::Int1 && t1 <= (int)TypeName::Uint8 && t2 >= (int)TypeName::Int1 && t2 <= (int)TypeName::Uint8)
            return TypeName::Int4;
        return TypeName::Error;
    }
    //Handle the logical op 
    regex logical_e(R"([!=<>]=|[!<>])");
    if(regex_match(op, logical_e)){
        return TypeName::Int4;
    }
    return TypeName(max((int)t1, (int)t2));
}
string toString(string src){
    Type t = symbol_table[src];
    switch(t.name){
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
        case TypeName:Uint8:
            return to_string(t.value.uint8);
        case TypeName::Float:
            return to_string(t.value.f);
        case TypeName::Double:
            return to_string(t.value.d);
        case TypeName::Pointer:
            return to_string((unsigned long long)t.value.ptr);
    }
}
void setVal(string s1, TypeName t1, string val){
    switch(t1){
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
            symbol_table[s1].value.ptr = (void*)stoull(val);
            break;
    }
}
void s_op2(string & des, string & src1, string & op, string & src2){
    TypeName l_type = symbol_table[des].name;
    TypeName r_type = typeCheck(src1, op, src2);
    
    if(r_type == TypeName::Error){
        //Error
        return;
    }
    if(l_type != r_type){
        //Error
        return;
    }
    string src1_val = isVar(src1) ? toString(src1) : src1;
    string src2_val = isVar(src2) ? toString(src2) : src2;
    
    regex cal_op_e(R"(+|-|\*|\/)");
    if(regex_match(op, cal_op_e)){
        if((int)r_type <= (int)TypeName::Int8){
            switch(op[0]){
                case '+':
                    setVal(des, r_type, to_string(stoll(src1_val) + stoll(src2_val)));
                    break;
                case '-':
                    setVal(des, r_type, to_string(stoll(src1_val) - stoll(src2_val)));
                    break;
                case '*':
                    setVal(des, r_type, to_string(stoll(src1_val) * stoll(src2_val)));
                    break;
                case '/':
                    setVal(des, r_type, to_string(stoll(src1_val) / stoll(src2_val)));
                    break;
            }
        }
        if((int)r_type <= (int)TypeName::Uint8 || r_type == TypeName::Pointer){
            switch(op[0]){
                case '+':
                    setVal(des, r_type, to_string(stoull(src1_val) + stoull(src2_val)));
                    break;
                case '-':
                    setVal(des, r_type, to_string(stoull(src1_val) - stoull(src2_val)));
                    break;
                case '*':
                    setVal(des, r_type, to_string(stoull(src1_val) * stoull(src2_val)));
                    break;
                case '/':
                    setVal(des, r_type, to_string(stoull(src1_val) / stoull(src2_val)));
                    break;
            }
        }
        if(r_type == TypeName::Float || r_type == TypeName::Double){
            switch(op[0]){
                case '+':
                    setVal(des, r_type, to_string(stod(src1_val) + stod(src2_val)));
                    break;
                case '-':
                    setVal(des, r_type, to_string(stod(src1_val) - stod(src2_val)));
                    break;
                case '*':
                    setVal(des, r_type, to_string(stod(src1_val) * stod(src2_val)));
                    break;
                case '/':
                    setVal(des, r_type, to_string(stod(src1_val) / stod(src2_val)));
                    break;
            }
        }
    }
}
void execute(int pc){
    auto t_ins = c_ins[pc];
    switch(get<0>(t_ins)){
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
            break;
        case 6:
            s_pnt_cpy(get<1>(t_ins), get<2>(t_ins));
            break;
        case 7:
            s_cpy_arr(get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
            break;
        case 8:
            // goto l1
            pc = label_table[get<1>(t_ins)];
            return;
        case 9:
        case 10:
            // param t1
            argStack.push(symbol_table[get<1>(t_ins)]);
            return;
        case 11:
            // call f1 3
            pcStack.push(pc);
            pc = label_table[get<1>(t_ins)];
            for(int i = stoi(get<2>(t_ins); i > 0; --i){
                string var_name = string("p") + to_string(i);
                symbol_table[var_name] = argStack.top();
                argStack.pop();
            }
            return;
        case 12:
            // return 
            pc = pcStack.top();
            pcStack.pop();
            if(get<1>(t_ins) != ""){
                string r_val = rValStack.top();
                rValStack.pop();
                s_cpy(r_val, get<1>(t_ins));
            }
            return;
        case 13:
            pcStack.push(pc);
            rValStack.push(get<1>(t_ins));
            pc = label_table[get<2>(t_ins)];
            for(int i = stoi(get<3>(t_ins); i > 0; --i){
                string var_name = string("p") + to_string(i);
                symbol_table[var_name] = argStack.top();
                argStack.pop();
            }
            return;
    }
}
int main(int argv, char **argc) {
    if (argv != 2)
        help();
    ifstream is(argc[1]);
    initialize(is);
    pc = label_table["f0"];//Start from main funciton
    while(1){
        execute(pc);
    }
    return 0;
}
