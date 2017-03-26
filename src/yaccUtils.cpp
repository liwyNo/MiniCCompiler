#include "yaccUtils.h"
#include <cstdlib>
#include <cstring>

Typename_t *newEnum(char *name, EnumTable_t *p)
{
    Typename_t *t = (Typename_t *)malloc(sizeof(Typename_t));
    t->type = idt_enum;
    t->name = NULL;
    t->structure = (IdStructure_t *)malloc(sizeof(IdStructure_t));
    t->structure->enumerate = p;
    StackAddTypename(t);
    return t;
}

void *memDup(const void *src, size_t size)
{
    return memcpy(malloc(sizeof(size)), src, size);
}
