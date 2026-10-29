#ifndef LOXGETTER_HPP
#define LOXGETTER_HPP

#include <string>

#include "loxcallable.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

class LoxGetter : public LoxCallable
{

    Getter_Field<void>* getter;
    Environment* closure {nullptr};

public:
    LoxGetter();
    LoxGetter(Getter_Field<void>* _getter);
    LoxGetter(Getter_Field<void>* _getter, Environment* _closure);


    Object call(Interpreter*, std::vector<Object>);
    std::size_t arity();
    std::string to_string();
    LoxGetter bind(Interpreter*, LoxInstance&);

};

#endif // LOXGETTER_HPP
