#include <iostream>

#include "environment.hpp"
#include "runtimeerror.hpp"

Environment::Environment() :
    values{},
    enclosing {nullptr}
    {}

Environment::Environment(Environment* env) :
    values {},
    enclosing {env}
    {}

void Environment::define_name(std::string name, Object value)
{
    values.insert(std::pair<std::string, Object>(name, value));
}

Object Environment::get(Token name)
{
    if(values.contains(name.get_lexeme())){
        return values.at(name.get_lexeme());
    }
    else if(enclosing){
        return enclosing->get(name);
    }
    throw RuntimeError(name, "Undefined variable '" + name.get_lexeme() + "'.\n");
}

void Environment::assign(Token name, const Object &value)
{
    if(values.contains(name.get_lexeme())){
        values[name.get_lexeme()] = value;
    }
    else if(enclosing){
        enclosing->assign(name, value);
    }
    else{
        throw RuntimeError(name, "Undefined variable '" + name.get_lexeme() +"'.\n");
    }
}
