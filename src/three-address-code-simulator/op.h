#include <string>
using namespace std;
void s_op2(string &des, string &src1, string &op, string &src2);
void s_op1(string &des, string &op, string &src1);
void s_cast(string &des, string &Var, string &src1);
void s_cpy(string &des, string &src);
void s_arr_cpy(string &ptr, string &offset, string src);
void s_pnt_cpy(string &ptr, string &src);
void s_cpy_arr(string &des, string &ptr, string offset);
void goto_exe(string & l, size_t & pc);
