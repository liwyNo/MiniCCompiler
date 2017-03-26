#include "yaccType.h"
#include <cstdlib>

void freeDD(direct_declarator_s_t *p)
{
    switch (p->type)
    {
        case 3:
            free(p->data.d3);
            break;
        case 4:
            free(p->data.d4.dd);
            break;
        case 5:
            free(p->data.d5.dd);
            break;
        case 6:
            free(p->data.d6);
            break;
        case 7:
            free(p->data.d7.dd);
            break;
    }
}
