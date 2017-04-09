#include "sim.h"
#include "op.h"
#include <string>
#include <cstdlib>
#include <unordered_map>
#include <regex>
using namespace std;

extern unordered_map<string, Var> symbol_table;
extern string * type_cstr;
extern unordered_map<string, TypeName> type_table;

TypeName getType(string src1);
void setVal(string &s1, TypeName t1, string val);
TypeName getType(string src1);
string toString(string src);

const int type_len[] = {1, 2, 4, 8, 1, 2, 4, 8, 4, 8, sizeof(void *)};

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
void s_cast(string &des, string &Var, string &src1) {
#ifdef DEBUG
    cout << "In s_cast:\t" << des << "\t" << Var << "\t" << src1 << endl;
    cout << "TypeName: " << type_cstr[(int)getType(des)] << "\t"
         << type_cstr[(int)getType(src1)];
#endif
    setVal(des, type_table[Var], toString(src1));
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
void goto_exe(string & l, size_t & pc){
    pc = (unsigned long long)symbol_table[l].value.ptr;
}
