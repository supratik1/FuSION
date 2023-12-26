#ifndef Z3MANAGER_H
#define Z3MANAGER_H

#include "z3++.h"

class Z3Solver
{
public:
    Z3Solver();
    ~Z3Solver();

    void test_method(z3::context &c, z3::solver &s);
};

#endif
