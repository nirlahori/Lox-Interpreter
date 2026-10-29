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

LoxClass::LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods, std::unordered_map<std::string, LoxFunction> _static_methods) :
    name{_name},
    methods{_methods},
    static_methods{_static_methods}
{}

LoxClass::LoxClass(std::string _name, std::unordered_map<std::string, LoxFunction> _methods, std::unordered_map<std::string, LoxFunction> _static_methods, std::unordered_map<std::string, LoxGetter> _getter_fields) :
    name{_name},
    methods{_methods},
    static_methods{_static_methods},
    getter_fields{_getter_fields}

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

LoxFunction LoxClass::find_method(std::string name, bool is_static)
{
    if(is_static){
        return static_methods.at(name);
    }
    return methods.at(name);
}

bool LoxClass::contains_method(std::string name, bool is_static)
{
    if(is_static){
        return static_methods.contains(name);
    }
    return methods.contains(name);
}

LoxFunction LoxClass::get_static_method(std::string name)
{
    if(contains_method(name, true)){
        return find_method(name, true);
    }
    return LoxFunction();
}

bool LoxClass::contains_getter_field(std::string name)
{
    return getter_fields.contains(name);
}

LoxGetter LoxClass::find_getter_field(std::string name)
{
    return getter_fields.at(name);
}

LoxInstance* LoxClass::create_instance(LoxClass* instance){
    instance_table.emplace_back(std::make_unique<LoxInstance>(*instance));
    return instance_table.back().get();
}
