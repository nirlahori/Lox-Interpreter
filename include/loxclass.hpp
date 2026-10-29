#ifndef LOXCLASS_HPP
#define LOXCLASS_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "loxcallable.hpp"
#include "loxfunction.hpp"
#include "loxgetter.hpp"


class LoxInstance;

class LoxClass : public LoxCallable
{
    std::string name;
    std::unordered_map<std::string, LoxFunction> methods;
    std::unordered_map<std::string, LoxFunction> static_methods;
    std::unordered_map<std::string, LoxGetter> getter_fields;

    static std::vector<std::unique_ptr<LoxInstance>> instance_table;
    static LoxInstance* create_instance(LoxClass*);

public:
    LoxClass();
    LoxClass(std::string _name);
    LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods);
    LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods, std::unordered_map<std::string, LoxFunction> _static_methods);
    LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods, std::unordered_map<std::string, LoxFunction> _static_methods, std::unordered_map<std::string, LoxGetter> _getter_fields);

    Object call(Interpreter*, std::vector<Object>);
    std::size_t arity();

    std::string to_string();
    LoxFunction find_method(std::string, bool is_static = false);
    bool contains_method(std::string, bool is_static = false);
    LoxFunction get_static_method(std::string name);
    bool contains_getter_field(std::string);
    LoxGetter find_getter_field(std::string);


};


#endif // LOXCLASS_HPP
