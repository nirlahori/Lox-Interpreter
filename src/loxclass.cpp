#include "loxclass.hpp"
#include "loxinstance.hpp"
#include "interpreter.hpp"

std::vector<std::unique_ptr<LoxInstance>> LoxClass::instance_table;

LoxClass::LoxClass() = default;

LoxClass::LoxClass(std::string _name) :
    name{_name}
{}

LoxClass::LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods) :
    name{_name},
    methods{_methods}
{}

Object LoxClass::call(Interpreter* intp, std::vector<Object> arguments)
{
    LoxInstance* instance {create_instance(this)};
    LoxFunction initializer;
    if(contains_method("init")){
        initializer = find_method("init");
        initializer.bind(intp, *instance).call(intp, arguments);
    }
    return *instance;
}

std::size_t LoxClass::arity()
{
    return contains_method("init") ? find_method("init").arity() : 0;
}

std::string LoxClass::to_string()
{
    return name;
}

LoxFunction LoxClass::find_method(std::string name)
{
    return methods.at(name);
}

bool LoxClass::contains_method(std::string name)
{
    return methods.contains(name);
}

LoxInstance* LoxClass::create_instance(LoxClass* instance){
    instance_table.emplace_back(std::make_unique<LoxInstance>(*instance));
    return instance_table.back().get();
}
