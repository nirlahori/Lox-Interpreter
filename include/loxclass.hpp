#ifndef LOXCLASS_HPP
#define LOXCLASS_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "loxcallable.hpp"
#include "loxfunction.hpp"


class LoxInstance;

class LoxClass : public LoxCallable
{
    std::string name;
    LoxClass* superclass;
    std::unordered_map<std::string, LoxFunction> methods;
    static std::vector<std::unique_ptr<LoxInstance>> instance_table;


    static LoxInstance* create_instance(LoxClass*);

public:
    LoxClass();
    LoxClass(std::string _name);
    LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods);
    LoxClass(std::string _name, LoxClass* _superclass, std::unordered_map<std::string, LoxFunction> _methods);

    Object call(Interpreter*, std::vector<Object>);
    std::size_t arity();

    std::string to_string();
    LoxFunction find_method(std::string);
    bool contains_method(std::string);
};


#endif // LOXCLASS_HPP
