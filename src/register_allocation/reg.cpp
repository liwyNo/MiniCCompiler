#include "reg.h"
#include <string>
#include <climits>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <algorithm>

RegSession::RegSession(int num)
{
    reg.resize(num);
}

void RegSession::detach(const std::string &var, int request_reg)
{
    auto &var_where = where[var];
    if (var_where.size() == 1 && !mem[var] && active[var])
        store(var);
    var_where.erase(std::find(var_where.begin(), var_where.end(), request_reg));
    auto &in_reg = reg[request_reg];
    in_reg.erase(std::find(in_reg.begin(), in_reg.end(), var));
}

void RegSession::detachAll(const std::string &var)
{
    if (!mem[var] && active[var])
        store(var);
    auto &var_where = where[var];
    for (auto &regi : var_where) {
        auto &in_reg = reg[regi];
        in_reg.erase(std::find(in_reg.begin(), in_reg.end(), var));
    }
    var_where.clear();
}

void RegSession::detachAll(int request_reg)
{
    auto &in_reg = reg[request_reg];
    for (auto &vari : in_reg) {
        auto &var_where = where[vari];
        if (var_where.size() == 1 && !mem[vari] && active[vari])
            store(vari);
        var_where.erase(std::find(var_where.begin(), var_where.end(), request_reg));
    }
    in_reg.clear();
}

void RegSession::link(const std::string &var, int request_reg)
{
    auto &var_where = where[var];
    if (std::find(var_where.begin(), var_where.end(), request_reg) != var_where.end())
        return;
    detachAll(request_reg);
    reg[request_reg].push_back(var);
    if (var_where.empty())
        load(var, request_reg);
    else
        move(var_where.front(), request_reg);
    var_where.push_back(request_reg);
}

int RegSession::storeCount(const std::vector<std::string> &v)
{
    int ans = 0;
    for (const auto &i : v)
        if (active[i] && !mem[i])
            ++ans;
    return ans;
}

int RegSession::findReg()
{
    int bestj, num = INT_MAX;
    for (size_t j = 0; j < reg.size(); ++j) {
        int numNow = storeCount(reg[j]);
        if (numNow < num) {
            bestj = j;
            num = numNow;
        }
    }
    return bestj;
}

int RegSession::findReg(const std::string &bad)
{
    int wbad = where[bad].size();
    int bestj, num = INT_MAX;
    for (size_t j = 0; j < reg.size(); ++j) {
        auto &regij = reg[j];
        int numNow = storeCount(regij);
        if (numNow < num && (wbad >= 2 || std::find(regij.begin(), regij.end(), bad) == regij.end())) {
            bestj = j;
            num = numNow;
        }
    }
    return bestj;
}

void RegSession::use(const std::string &var)
{
    active[var] = true;
    if (where[var].empty())
        link(var, findReg());
}

void RegSession::use(const std::string &var1, const std::string &var2)
{
    active[var1] = active[var2] = true;
    auto *w1 = &where[var1], *w2 = &where[var2];
    if (w1->size() && w2->size())
        return;
    const std::string *v1, *v2;
    if (w2->size())
        v1 = &var2, v2 = &var1, swap(w1, w2);
    else
        v1 = &var1, v2 = &var2;
    if (w1->empty())
        use(*v1);
    link(*v2, findReg(*v1));
}

void RegSession::def(const std::string &var)
{
    use(var);
    mem[var] = false;
}

void RegSession::inactive(const std::string &var)
{
    active[var] = false;
}

int RegSession::get(const std::string &var)
{
    auto &var_where = where[var];
    if (var_where.empty())
        use(var);
    return var_where.front();
}

void RegSession::load(const std::string &var, int request_reg)
{
    mem[var] = true;
    printf("load %s r%d\n", var.c_str(), request_reg);
}

void RegSession::store(const std::string &var)
{
    mem[var] = true;
    const auto &var_reg = where[var].front();
    printf("store %s r%d\n", var.c_str(), var_reg);
}

void RegSession::move(int reg_src, int reg_dest)
{
    printf("move r%d r%d\n", reg_src, reg_dest);
}

int main()
{
    std::string line;
    RegSession rs(3);
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string op;
        ss >> op;
        if (op == "use") {
            std::string u1, u2;
            if (ss >> u1 >> u2)
                rs.use(u1, u2);
            else
                rs.use(u1);
        }
        else if (op == "def") {
            std::string u;
            ss >> u;
            rs.def(u);
        }
        else if (op == "inactive") {
            std::string u;
            ss >> u;
            rs.inactive(u);
        }
        else if (op == "get") {
            std::string u;
            ss >> u;
            int reg = rs.get(u);
            printf("reg r%d\n", reg);
        }
    }
    return 0;
}
