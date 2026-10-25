#ifndef LOXFUNCTION_HPP
#define LOXFUNCTION_HPP

#include "loxcallable.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include "object.hpp"

class LoxInstance;

class LoxFunction : public LoxCallable
{

    Function<void>* decl;
    Environment* closure = nullptr;
    bool is_initializer;

public:
    LoxFunction();
    LoxFunction(Function<void>* _decl);
    LoxFunction(Function<void>* _decl, Environment* _closure);
    LoxFunction(Function<void>* _decl, Environment* _closure, bool _is_initializer);

    Object call(Interpreter* intp, std::vector<Object> arguments);
    std::size_t arity();
    std::string to_string();
    LoxFunction bind(Interpreter* intp, LoxInstance&);
};

#endif // LOXFUNCTION_HPP
