#ifndef LOXINSTANCE_HPP
#define LOXINSTANCE_HPP

#include <string>
#include <unordered_map>

#include "loxclass.hpp"
#include "object.hpp"
#include "token.hpp"
#include "interpreter.hpp"

class LoxInstance
{
    LoxClass klass;
    std::unordered_map<std::string, Object> fields;

public:
    LoxInstance();
    LoxInstance(LoxClass _klass);

    LoxInstance(const LoxInstance& other);
    LoxInstance(LoxInstance&& other);

    LoxInstance& operator = (const LoxInstance& other);
    LoxInstance& operator = (LoxInstance&& other);
    Object get(Interpreter* intp, Token name);
    void set(Token name, Object value);
    std::string to_string();
    void set_class(LoxClass _klass);

};

#endif // LOXINSTANCE_HPP
