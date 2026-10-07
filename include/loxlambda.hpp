#ifndef LOXLAMBDA_HPP
#define LOXLAMBDA_HPP


#include "loxcallable.hpp"
#include "expr.hpp"
#include "environment.hpp"

struct LoxLambda : public LoxCallable
{

    Lambda<Object>* func;
    Environment* closure;

public:
    LoxLambda();
    LoxLambda(Lambda<Object>*, Environment*);

    Object call(Interpreter*, std::vector<Object>);
    std::size_t arity();
    std::string to_string();


};

#endif // LOXLAMBDA_HPP
