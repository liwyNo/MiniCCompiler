#ifndef YACCTYPES_H
#define YACCTYPES_H

#include <string>
#include <cstring>
#include "tigger.h"

bool check_op2(const char *s);
bool check_op1(const char *s);
bool check_rop(const char *s);

struct statement_t {
    virtual void run() = 0;
};

struct stmt_func_begin: public statement_t {
    std::string name;
    int argNum;
    int stackSlotNum;
    stmt_func_begin(const std::string &_name, int _argNum, int _stackSlotNum): name(_name), argNum(_argNum), stackSlotNum(_stackSlotNum) {}
    void run();
};

struct stmt_func_end: public statement_t {
    std::string name;
    stmt_func_end(const std::string &_name): name(_name) {}
    void run();
};

struct stmt_assign_const: public statement_t {
    int rnum;
    int value;
    stmt_assign_const(int _rnum, int _value): rnum(_rnum), value(_value) {}
    void run();
};

struct stmt_assign_op2: public statement_t {
    int dest;
    int src1, src2;
    std::string op;
    stmt_assign_op2(int _dest, int _src1, const char *_op, int _src2): dest(_dest), src1(_src1), src2(_src2), op(_op) {}
    void run();
};

struct stmt_assign_op1: public statement_t {
    int dest;
    int src;
    std::string op;
    stmt_assign_op1(int _dest, const char *_op, int _src): dest(_dest), src(_src), op(_op) {}
    void run();
};

struct stmt_assign_lidx: public statement_t {
    int dest;
    int idx;
    int src;
    stmt_assign_lidx(int _dest, int _idx, int _src): dest(_dest), idx(_idx), src(_src) {}
    void run();
};

struct stmt_assign_move: public statement_t {
    int dest;
    int src;
    stmt_assign_move(int _dest, int _src): dest(_dest), src(_src) {}
    void run();
};

struct stmt_assign_ridx: public statement_t {
    int dest;
    int src;
    int idx;
    stmt_assign_ridx(int _dest, int _src, int _idx): dest(_dest), src(_src), idx(_idx) {}
    void run();
};

struct stmt_if_goto: public statement_t {
    int src1, src2;
    std::string op;
    int lnum;
    stmt_if_goto(int _src1, const char *_op, int _src2, int _lnum): src1(_src1), src2(_src2), op(_op), lnum(_lnum) {}
    void run();
};

struct stmt_goto: public statement_t {
    int lnum;
    stmt_goto(int _lnum): lnum(_lnum) {}
    void run();
};

struct stmt_label: public statement_t {
    int lnum;
    stmt_label(int _lnum): lnum(_lnum) {}
    void run();
};

struct stmt_call: public statement_t {
    std::string name;
    stmt_call(const char *_name): name(_name) {}
    void run();
};

struct stmt_store_local: public statement_t {
    int rnum;
    int snum;
    stmt_store_local(int _rnum, int _snum): rnum(_rnum), snum(_snum) {}
    void run();
};

struct stmt_store_global: public statement_t {
    int rnum;
    int xnum;
    stmt_store_global(int _rnum, int _xnum): rnum(_rnum), xnum(_xnum) {}
    void run();
};

struct stmt_load_local: public statement_t {
    int snum;
    int rnum;
    stmt_load_local(int _snum, int _rnum): snum(_snum), rnum(_rnum) {}
    void run();
};

struct stmt_load_global: public statement_t {
    int xnum;
    int rnum;
    stmt_load_global(int _xnum, int _rnum): xnum(_xnum), rnum(_rnum) {}
    void run();
};

struct stmt_malloc: public statement_t {
    int rnum;
    int size;
    stmt_malloc(int _rnum, int _size): rnum(_rnum), size(_size) {}
    void run();
};

void add_func_begin(const std::string &fname, int argNum, int stackSlotNum);
void add_label(int lnum);
void add_gvar_int(int xnum, int value);
void add_gvar_array(int xnum, int size);

#endif