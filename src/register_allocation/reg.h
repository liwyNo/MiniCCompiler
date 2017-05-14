#ifndef REG_H
#define REG_H

#include <map>
#include <string>
#include <vector>

#define CALLEE_SAVE 0
#define CALLER_SAVE 1

class RegSession
{
private:
    std::vector<std::vector<std::string>> reg;
    std::map<std::string, std::vector<int>> where;
    std::map<std::string, bool> mem, active;

    void detach(const std::string &var, int request_reg);
    void detachAll(const std::string &var);
    void detachAll(int request_reg);
    void link(const std::string &var, int request_reg);
    int findReg();
    int findReg(const std::string &bad);
    int storeCount(const std::vector<std::string> &);
    void load(const std::string &var, int request_reg);
    void store(const std::string &var);
    void move(int reg_src, int reg_dest);

public:
    RegSession(int num);
    void use(const std::string &var);
    void use(const std::string &var1, const std::string &var2);
    void def(const std::string &var);
    void inactive(const std::string &var);
    int get(const std::string &var);

};

/* example:
 * x = x + y
 *
 * use x, y
 * r1 = get(x)
 * r2 = get(y)
 * inactive(x)  (optional but recommended)
 * inactive(y)  (optional)
 * def x
 * r3 = get(x)
 * gen(r3 = r1 + r2)
 */

#endif // REG_H
