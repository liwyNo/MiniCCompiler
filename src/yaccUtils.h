#ifndef YACCUTILS_H
#define YACCUTILS_H

#include "MiniC.tab.hpp"
#include "symbol.h"

Typename_t *newEnum(char *, EnumTable_t *);

template<class T> T *memDup(const T *src)
{
    return (T*)memcpy(new T, src, sizeof(T));
}

void genDeclare(const Identifier_t *, bool global);

#endif
