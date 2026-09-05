#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <unordered_map>
#include <string>

#include "object.hpp"
#include "token.hpp"

class Environment
{

    std::unordered_map<std::string, Object> values;
    Environment* enclosing;

public:
    Environment();
    Environment(Environment* env);
    void define_name(std::string name, Object value);
    Object get(Token name);
    void assign(Token name, const Object& value);
};

#endif // ENVIRONMENT_HPP
