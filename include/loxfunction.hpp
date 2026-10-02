#ifndef LOXFUNCTION_HPP
#define LOXFUNCTION_HPP

#include <optional>

#include "loxcallable.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include "object.hpp"

class LoxFunction : public LoxCallable
{

    Function<void>* decl;
    std::optional<Environment> closure;

public:
    LoxFunction();
    LoxFunction(Function<void>* _decl);
    LoxFunction(Function<void>* _decl, std::optional<Environment> _closure);

    Object call(Interpreter* intp, std::vector<Object> arguments);
    std::size_t arity();
    std::string to_string();
    void set_closure(std::optional<Environment> _closure);
};

#endif // LOXFUNCTION_HPP
