#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
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
    String
};
struct Type {
    TypeName name;
    UnionType value;
    size_t length;
    Type(){
        length = 0;
        value.uint8 = 0;
    }
};
regex label_regex(R"(^\s*([fl]\d)+:\s*(?:\/\/.*)?$)");
regex dec_regex(
    R"(^\s*g?var\s+(u?int[1248]|float[48]|ptr)\s+(?:([1-9]\d*)\s+)?([Ttp]\d+)\s*(?:\/\/.*)?$)");
regex const_dec_regex(
    R"(^\s*const\s+(u?int[1248]|float[48]|ptr|str)\s+(c\d+)\s+(".*"|.*)\s*(?:\/\/.*)?$)");
regex int_regex(R"(^\d+$)");
regex float_regex(R"(^\d+\.\d+)");

// //comment
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
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpcf]\d+)\s*(?:\/\/.*)?$)");
// t1[t2]=t3
regex exp5_regex(
    R"(^\s*([Ttpc]\d+)\s*\[\s*([Ttpc]\d+|\d+)\s*\]\s*=\s*([Ttpc]\d+)\s*(?:\/\/.*)?$)");
//*t1=t2
regex exp6_regex(
    R"(^\s*\*\s*([Ttpc]\d+)\s*=\s*([Ttpc]\d+)\s*(?:\/\/.*)?$)");
// t1=t2[t3]
regex exp7_regex(
    R"(^\s*([Ttp]\d+)\s*=\s*([Ttpc]\d+)\s*\[\s*([Ttpc]\d+|\d*)\s*\]\s*(?:\/\/.*)?$)");
regex goto_regex(R"(^\s*goto\s+([lf]\d+)\s*(?:\/\/.*)?$)");
regex if_regex(
    R"(^\s*if\s+([Ttpc]\d+)\s*([<>]|[!<>=]=)\s*([Ttpc]\d+)\s+goto\s+([lf]\d+)\s*(?:\/\/.*)?$)");
regex param_regex(R"(^\s*param\s+([lf]\d+)\s*(?:\/\/.*)?$)");
regex call_regex(R"(^\s*call\s+([lf]\d+)\s+(\d+)\s*(?:\/\/.*)?$)");
regex call2_regex(
    R"(^\s*([tTp]\d+)\s*=\s*call\s+([lf]\d+)\s+(\d+)\s*(?:\/\/.*)?$)");
regex return_regex(R"(^\s*return(?:\s+([Ttp]\d+))?\s*(?:\/\/.*)?$)");

const string type_cstr[] = {"int1",   "int2",   "int4",  "int8",
                            "uint1",  "uint2",  "uint4", "uint8",
                            "float4", "float8", "ptr",   "str"};
const int type_len[] = {1, 2, 4, 8, 1, 2, 4, 8, 4, 8, sizeof(void *)};
size_t pc;                                     // Next instruction
vector<string> ins;                         // To store all instruction
unordered_map<string, TypeName> type_table; // Map the string name to TypeName
//unordered_map<string, int> label_table;     // Map the label name to line number
unordered_map<string, Type>
    symbol_table; // Map the symbol name to value and type
vector<tuple<int, string, string, string, string>> c_ins;

TypeName getType(string src1) { return symbol_table[src1].name; }
string toString(string src) {
    Type t = symbol_table[src];
    switch (t.name) {
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
void build_label_table() {
    // Initialize the label table
    smatch match_result;
    int lineCounter = 0;
    for (auto x : ins) {
        if (regex_match(x, match_result, label_regex)) {
            string match_str = match_result[1].str();
            if (symbol_table.find(match_str) == symbol_table.end()) {
                Type t;
                t.name = TypeName::Pointer;
                t.value.uint8 = lineCounter + 1;
                symbol_table[match_str] = t; // Label point to next line
            } else {
                // Error - same label
            }
        }
        lineCounter++;
    }
#ifdef DEBUG
    // Print the label table
    cout << "\tLabel Table" << endl;
    cout << "label\tline\tvalue" << endl;
    for (auto x : symbol_table)
        cout << x.first << "\t" << x.second.value.uint8 << "\t" << toString(x.first) << endl;
#endif
}
void build_symbol_table() {
    smatch match_result;
    for (auto x : ins) {
        if (regex_match(x, match_result, dec_regex)) {
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
            if (regex_match(x, match_result, const_dec_regex)) {
                // Get the match str
                string type_str = match_result[1].str();
                string name_str = match_result[2].str();
                string value_str = match_result[3].str();

                // Analyze the type
                TypeName type_name = type_table[type_str];

                Type t;
                t.name = type_name;
                if (symbol_table.find(name_str) != symbol_table.end()) {
                    // Error
                    cerr << "Constant decleration duplication: " << name_str << endl;
                    exit(-1);
                }
                switch (type_name) {
                case TypeName::String: {
                    t.value.ptr =
                        new char[value_str.length() - 1]; //"xxx" length = 5
                    sscanf((char *)t.value.ptr, "\"%s\"",
                           (char *)value_str.c_str());
                    t.length = value_str.length() - 2;
                    t.name = TypeName::Pointer;
                    break;
                }
                default:
                    setVal(name_str, t.name, value_str);
                    break;
                }
                symbol_table[name_str] = t;
            }
        }
    }
#ifdef DEBUG
    // Print the symbol table
    cout << "\tSymbol Table" << endl;
    cout << "symbol\ttype\tlength\tvalue" << endl;
    for (auto x : symbol_table) {
        cout << x.first << "\t" << type_cstr[int(x.second.name)] << "\t"
             << x.second.length << "\t" << toString(x.first) << endl;
    }
#endif
}
void compile_ins(){
    smatch match_result;
    // check the return type of "t1 = t2 op t3"
    auto returnType = [](string&& src1, string && op, string && src2){
        TypeName t2 = getType(src2);
        // Handle some op can only use between two integer
        regex intop_e(R"(^[<>]|[<>!=]=$)");
        if (regex_match(op, intop_e))
            return TypeName::Int4;
        return t2;
    };
    // check the type of "t1 = t2 op t2"
    auto typeCheck = [](string &&src1, string &&op, string &&src2) {
        TypeName t1 = getType(src1), t2 = getType(src2);
        // Handle ptr
        if (t1 != t2)
            return false;
        // Handle some op can only use between two integer
        regex intop_e(R"([%^|&]|\|\||&&|<<|>>)");
        if (regex_match(op, intop_e)) {
            if (t1 >= TypeName::Int1 && t1 <= TypeName::Uint8)
                return true;
            return false;
        }
        return true;
    };
    // compile the instruction
    for (auto x : ins) {
        int match_count = 0, match_type = 0;
        cout << x << endl;
        do {
            if ((x == "") | regex_match(x, comment_regex) | regex_match(x, label_regex) |
                regex_match(x, dec_regex) | regex_match(x, const_dec_regex))
                break;
            if (regex_match(x, match_result, exp1_regex)) {
                if ((!typeCheck(match_result[2].str(), match_result[3].str(),
                                match_result[4].str())) or
                    (getType(match_result[1].str()) !=
                     returnType(match_result[2].str(), match_result[3].str(),
                                match_result[4].str()))) {
                    // ERROR
                }
                match_count = 4;
                match_type = 1;
                break;
            }
            if (regex_match(x, match_result, exp2_regex)) {
                if ((match_result[2].str() == "*" and
                     symbol_table[match_result[2].str()].name !=
                         TypeName::Pointer) or
                    (match_result[2].str() == "&" and
                     symbol_table[match_result[2].str()].name >
                         TypeName::Uint8 and
                     symbol_table[match_result[1].str()].name !=
                         TypeName::Pointer)) {
                    // ERROR
                }
                match_count = 3;
                match_type = 2;
                break;
            }
            if (regex_match(x, match_result, exp3_regex)) {
                if (getType(match_result[1].str()) !=
                    getType(match_result[2].str())) {
                    // Error
                }
                match_count = 3;
                match_type = 3;
                break;
            }
            if (regex_match(x, match_result, exp4_regex)) {
                if (getType(match_result[1].str()) !=
                    getType(match_result[2].str())) {
                    // Error
                }
                match_count = 2;
                match_type = 4;
                break;
            }
            if (regex_match(x, match_result, exp5_regex)) {
                if (getType(match_result[2].str()) > TypeName::Uint8 or
                    getType(match_result[1].str()) != TypeName::Pointer) {
                    // Error
                }
                match_count = 3;
                match_type = 5;
                break;
            }
            if (regex_match(x, match_result, exp6_regex)) {
                if (getType(match_result[1].str()) != TypeName::Pointer) {
                    // Error
                }
                match_count = 2;
                match_type = 6;
                break;
            }
            if (regex_match(x, match_result, exp7_regex)) {
                if (getType(match_result[2].str()) != TypeName::Pointer or
                    getType(match_result[3].str()) < TypeName::Uint8) {
                    // Error
                }
                match_count = 3;
                match_type = 7;
                break;
            }
            if (regex_match(x, match_result, goto_regex)) {
                match_count = 1;
                match_type = 8;
                break;
            }
            if (regex_match(x, match_result, if_regex)) {
                match_count = 4;
                match_type = 9;
                break;
            }
            if (regex_match(x, match_result, param_regex)) {
                match_count = 1;
                match_type = 10;
                break;
            }
            if (regex_match(x, match_result, call_regex)) {
                match_count = 2;
                match_type = 11;
                break;
            }
            if (regex_match(x, match_result, return_regex)) {
                match_count = 1;
                match_type = 12;
                break;
            }
            if (regex_match(x, match_result, call2_regex)) {
                match_count = 3;
                match_type = 13;
                break;
            }

            // Error no type match!
            cerr << "Ins:\"" << x << "\" can't compile." << endl;
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
    // Initialize the symbol table

    build_symbol_table();

    compile_ins();
    return true;
}
stack<Type> argStack;
stack<size_t> pcStack;
stack<string> rValStack;
void help() {}

template <typename T> void exe1_op2(string &des, string &op, T val1, T val2) {
    TypeName t1 = getType(des);
    switch (op.length()) {
    case 1:
        switch (op[0]) {
        case '+':
            setVal(des, t1, to_string(val1 + val2));
            break;
        case '-':
            setVal(des, t1, to_string(val1 - val2));
            break;
        case '*':
            setVal(des, t1, to_string(val1 * val2));
            break;
        case '/':
            setVal(des, t1, to_string(val1 / val2));
            break;
        case '<':
            setVal(des, TypeName::Int4, to_string(val1 < val2));
            break;
        case '>':
            setVal(des, TypeName::Int4, to_string(val1 > val2));
            break;
        }
        break;
    case 2:
        switch (op[0]) {
        case '>':
            setVal(des, TypeName::Int4, to_string(val1 >= val2));
            break;
        case '<':
            setVal(des, TypeName::Int4, to_string(val1 <= val2));
            break;
        case '=':
            setVal(des, TypeName::Int4, to_string(val1 == val2));
            break;
        case '!':
            setVal(des, TypeName::Int4, to_string(val1 != val2));
            break;
        case '|':
            setVal(des, TypeName::Int4, to_string(val1 || val2));
            break;
        case '&':
            setVal(des, TypeName::Int4, to_string(val1 && val2));
            break;
        }
    }
}
void s_op2(string &des, string &src1, string &op, string &src2) {
    TypeName t1 = getType(des);
    TypeName t2 = getType(src1);
    regex opt1(R"(^[!=<>]=|-|[+*/<>]|\|\||&&$)");
    if (regex_match(op, opt1)) {
        if (t2 == TypeName::Float || t2 == TypeName::Double) {
            double val1 = stod(toString(src1)), val2 = stod(toString(src2));
            exe1_op2(des, op, val1, val2);
        }
        if (t2 <= TypeName::Int8) {
            long long val1 = stoll(toString(src1)),
                      val2 = stoll(toString(src2));
            exe1_op2(des, op, val1, val2);
        }
        if ((t2 > TypeName::Int8 && t2 <= TypeName::Uint8) ||
            t2 == TypeName::Pointer) {
            unsigned long long val1 = stoull(toString(src1)),
                               val2 = stoull(toString(src2));
            exe1_op2(des, op, val1, val2);
        }
        return;
    } else {
        if (op == "%") {
            if (t2 <= TypeName::Int8) {
                long long val1 = stoll(toString(src1)),
                          val2 = stoll(toString(src2));
                setVal(des, t1, to_string(val1 % val2));
            } else {
                unsigned long long val1 = stoull(toString(src1)),
                                   val2 = stoull(toString(src2));
                setVal(des, t1, to_string(val1 % val2));
            }
            return;
        }
        unsigned long long val1 = stoull(toString(src1)),
                           val2 = stoull(toString(src2));
        if (op.length() == 1) {
            switch (op[0]) {
            case '^':
                setVal(des, t1, to_string(val1 ^ val2));
                break;
            case '|':
                setVal(des, t1, to_string(val1 | val2));
                break;
            case '&':
                setVal(des, t1, to_string(val1 & val2));
                break;
            }
        } else {
            switch (op[0]) {
            case '<':
                setVal(des, t1, to_string(val1 < val2));
                break;
            case '>':
                setVal(des, t1, to_string(val1 > val2));
                break;
            }
        }
    }
}
void s_op1(string &des, string &op, string &src1) {
    TypeName t2 = getType(src1);
    TypeName t1 = getType(des);
#ifdef DEBUG
    cout << "In s_op1:\t" << des << "\t" << op << "\t" << src1 << endl;
    cout << "TypeName: " << type_cstr[(int)t1] << "\t" << type_cstr[(int)t2];
#endif
    switch (op[0]) {
    case '*':
        void s_cpy_arr(string &, string &, string);
        s_cpy_arr(des, src1, "");
        break;
    case '&':
        setVal(des, TypeName::Pointer,
               to_string((unsigned long long)&symbol_table[src1].value));
        break;
    case '-':
        setVal(des, t1, string("-") + toString(src1));
        break;
    case '!':
        setVal(des, TypeName::Int4, to_string(!symbol_table[src1].value.uint8));
        break;
    case '~':
        setVal(des, t1, to_string(~symbol_table[src1].value.uint8));
        break;
    }
}
void s_cast(string &des, string &type, string &src1) {
#ifdef DEBUG
    cout << "In s_cast:\t" << des << "\t" << type << "\t" << src1 << endl;
    cout << "TypeName: " << type_cstr[(int)getType(des)] << "\t"
         << type_cstr[(int)getType(src1)];
#endif
    setVal(des, type_table[type], toString(src1));
}
void s_cpy(string &des, string &src) {
#ifdef DEBUG
    cout << "In s_cpy:\t" << des << "\t" << src << endl;
#endif
    symbol_table[des] = symbol_table[src];
}
void s_arr_cpy(string &ptr, string &offset, string src) {
#ifdef DEBUG
    cout << "In s_arr_cpy:\t" << ptr << "\t" << offset << "\t" << src << endl;
#endif
    size_t off = stoull(toString(offset));
    off += (size_t)symbol_table[ptr].value.ptr;
    size_t cpy_size = type_len[(int)getType(src)];
    memcpy((void *)off, (void *)&symbol_table[src].value, cpy_size);
}
void s_pnt_cpy(string &ptr, string &src) {
#ifdef DEBUG
    cout << "In s_arr_cpy:\t" << ptr << "\t" << src << endl;
#endif
    size_t cpy_size = type_len[(int)getType(src)];
    memcpy((void *)&symbol_table[ptr].value, (void *)&symbol_table[src].value,
           cpy_size);
}
void s_cpy_arr(string &des, string &ptr, string offset) {
#ifdef DEBUG
    cout << "In s_cpy_arr:\t" << des << "\t" << ptr << "\t" << offset << endl;
#endif
    size_t off = 0;
    off = offset == "" ? 0 : stoull(toString(offset));
    off += (size_t)symbol_table[ptr].value.ptr;
    size_t cpy_size = type_len[(int)getType(des)];
    memcpy((void *)&symbol_table[des].value, (void *)off, cpy_size);
}
void execute(int pc_l) {
#ifdef DEBUG
    cout << "pc: " << pc_l << endl;
#endif
    auto t_ins = c_ins[pc_l];
    pc ++;
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
        pc = (unsigned long long)symbol_table[get<1>(t_ins)].value.ptr;
        return;
    case 9:{
            string tmp("tmp");
            symbol_table[tmp].value.int4 = 0;
            s_op2(tmp, get<1>(t_ins), get<2>(t_ins), get<3>(t_ins));
            if(symbol_table["tmp"].value.int4){
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
        pcStack.push(pc);
        pc = (unsigned long long)symbol_table[get<1>(t_ins)].value.ptr;
        for (int i = stoi(get<2>(t_ins)); i > 0; --i) {
            string var_name = string("p") + to_string(i);
            symbol_table[var_name] = argStack.top();
            argStack.pop();
        }
        return;
    case 12:
        // return
        pc = pcStack.top();
        pcStack.pop();
        if (get<1>(t_ins) != "") {
            string r_val = rValStack.top();
            rValStack.pop();
            s_cpy(r_val, get<1>(t_ins));
        }
        return;
    case 13:
        pcStack.push(pc);
        rValStack.push(get<1>(t_ins));
        pc = (unsigned long long)symbol_table[get<2>(t_ins)].value.ptr;
        for (int i = stoi(get<3>(t_ins)); i > 0; --i) {
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
    pc = (unsigned long long)symbol_table["f0"].value.ptr;
    pcStack.push(-1);
#ifdef DEBUG
    cout << "BEGIN PROGRAM" << endl;
#endif
    while (pc != (unsigned long long) -1) {
        execute(pc);
    }
    return 0;
}
