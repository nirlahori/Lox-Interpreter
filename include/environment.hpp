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
    Environment* ancestor(int distance);

public:
    Environment();
    Environment(Environment* env);
    void define_name(std::string name, const Object& value);
    Object get(Token name);
    void assign(Token name, const Object& value);
    Object get_at(int distance, std::string name);
    void assign_at(int distance, Token name, const Object& value);
};

#endif // ENVIRONMENT_HPP
