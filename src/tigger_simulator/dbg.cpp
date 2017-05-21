#include "dbg.h"
#include "tigger.h"
#include "yaccTypes.h"
#include <cstring>
#include <cstdlib>

bool debug;
int run_type = DEBUG_NEXT;
std::set<int> breaks;

void debug_wait()
{
    if (!debug)
        return;
    if (run_type == DEBUG_CONTINUE)
        if (breaks.find(stmts[pc]->line) == breaks.end())
            return;
    while (1) {
        printf("> ");
        fflush(stdout);
        static char line[100];
read_again:
        gets(line);
        static char op[100];
        if (sscanf(line, "%s", op) != 1)
            goto read_again;
        statement_t *ptr = stmts[pc];
        if (strcmp(op, "l") == 0)
            printf("line %d\n", ptr->line);
        else if (strcmp(op, "n") == 0) {
            run_type = DEBUG_NEXT;
            return;
        }
        else if (strcmp(op, "pr") == 0) {
            static char r[100];
            sscanf(line, "%s %s", op, r);
            int rnum = find_reg(r);
            if (rnum == -1)
                printf("no register %s\n", r);
            else
                printf("%s = %d\n", r, reg[rnum]);
        }
        else if (strcmp(op, "prx") == 0) {
            static char r[100];
            sscanf(line, "%s %s", op, r);
            int rnum = find_reg(r);
            if (rnum == -1)
                printf("no register %s\n", r);
            else
                printf("%s = 0x%x\n", r, reg[rnum]);
        }
        else if (strcmp(op, "ps") == 0) {
            int snum;
            sscanf(line, "%s %d", op, &snum);
            if (snum >= ssp)
                printf("%d is out of stack space\n", snum);
            else
                printf("stack[%d] = %d\n", snum, stackmem[sp - ssp + snum]);
        }
        else if (strcmp(op, "psx") == 0) {
            int snum;
            sscanf(line, "%s %d", op, &snum);
            if (snum >= ssp)
                printf("%d is out of stack space\n", snum);
            else
                printf("stack[%d] = 0x%x\n", snum, stackmem[sp - ssp + snum]);
        }
        else if (strcmp(op, "pg") == 0) {
            int xnum;
            sscanf(line, "%s %d", op, &xnum);
            if (gvar_name.find(xnum) == gvar_name.end())
                printf("no global variable %d\n", xnum);
            else
                printf("%d = %d\n", xnum, gvars[gvar_name[xnum]]);
        }
        else if (strcmp(op, "b") == 0) {
            int bnum;
            sscanf(line, "%s %d", op, &bnum);
            breaks.insert(bnum);
            printf("set breakpoint at line %d\n", bnum);
        }
        else if (strcmp(op, "d") == 0) {
            int bnum;
            sscanf(line, "%s %d", op, &bnum);
            breaks.erase(bnum);
            printf("delete breakpoint at line %d\n", bnum);
        }
        else if (strcmp(op, "c") == 0) {
            run_type = DEBUG_CONTINUE;
            return;
        }
        else if (strcmp(op, "q") == 0) {
            exit(0);
        }
        else
            printf("unknown operation '%s'\n", op);
    }
}