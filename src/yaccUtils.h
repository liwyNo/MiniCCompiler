#ifndef YACCUTILS_H
#define YACCUTILS_H

#include "MiniC.tab.hpp"
#include "symbol.h"

Typename_t *newEnum(char *, EnumTable_t *);
void *memDup(const void *, size_t);

#endif
