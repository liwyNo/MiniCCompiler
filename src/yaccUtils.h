#ifndef YACCUTILS_H
#define YACCUTILS_H

#include "MiniC.tab.hpp"
#include "symbol.h"

Typename_t *newEnum(char *, EnumTable_t *);

template<class T> T *memDup(const T *src)
{
    return (T*)memcpy(new T, src, sizeof(T));
}

void arrayLengthSum(const_Typename_ptr);

void genDeclare(const_Typename_ptr type, const char *TACname, bool global);
void genInitilize(const_Typename_ptr, const char *TACname, const initializer_s_t*);

#endif
