#include "loxgetter.hpp"
#include "returnvalue.hpp"
#include "loxinstance.hpp"


LoxGetter::LoxGetter() = default;

LoxGetter::LoxGetter(Getter_Field<void> *_getter) :
    getter{_getter}
    {}

LoxGetter::LoxGetter(Getter_Field<void> *_getter, Environment *_closure) :
    getter{_getter},
    closure{_closure}
    {}

Object LoxGetter::call(Interpreter* intp, [[maybe_unused]] std::vector<Object> arguments)
{
    Environment* env {intp->create_environment(closure)};
    try{
        intp->execute_block(getter->body, *env);
    }
    catch(ReturnValue& rv){
        return rv.value;
    }
    return nullptr;
}

std::size_t LoxGetter::arity()
{
    return 0;
}

std::string LoxGetter::to_string()
{
    return "<getter " + getter->name.get_lexeme() + " >";
}

LoxGetter LoxGetter::bind(Interpreter* intp, LoxInstance& instance)
{
    Environment* environment {intp->create_environment(closure)};
    environment->define_name("this", instance);
    return LoxGetter(getter, environment);
}
