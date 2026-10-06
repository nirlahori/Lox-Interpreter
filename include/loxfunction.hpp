#ifndef LOXFUNCTION_HPP
#define LOXFUNCTION_HPP

#include "loxcallable.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include "object.hpp"

class LoxFunction : public LoxCallable
{

    Function<void>* decl;
    Environment* closure = nullptr;

public:
    LoxFunction();
    LoxFunction(Function<void>* _decl);
    LoxFunction(Function<void>* _decl, Environment* _closure);


    Object call(Interpreter* intp, std::vector<Object> arguments);
    std::size_t arity();
    std::string to_string();
};

#endif // LOXFUNCTION_HPP
