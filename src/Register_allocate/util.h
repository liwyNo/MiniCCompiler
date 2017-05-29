#ifndef UTIL_H
#define UTIL_H
#include <string>
#include "typedef.h"
#include "analysis.h"

std::string fix_name(std::string var_name, Function *now_fun);
void checkNotArray(Variable *this_var);
void checkArray(Variable *this_var);
void init_debug();
#endif