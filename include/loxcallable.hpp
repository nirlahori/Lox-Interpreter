#ifndef LOXCALLABLE_HPP
#define LOXCALLABLE_HPP

#include <vector>

#include "object.hpp"
#include "interpreter.hpp"

struct LoxCallable
{
    LoxCallable() = default;
    virtual Object call(Interpreter*, std::vector<Object>) = 0;
    virtual std::size_t arity() = 0;
};



#endif // LOXCALLABLE_HPP
