#include "loxfunction.hpp"
#include "returnvalue.hpp"
#include "loxinstance.hpp"
#include "interpreter.hpp"

LoxFunction::LoxFunction() = default;

LoxFunction::LoxFunction(Function<void> *_decl) :
    decl{_decl}
    {}

LoxFunction::LoxFunction(Function<void>* _decl, Environment* _closure) :
    decl{_decl},
    closure{_closure}
    {}

LoxFunction::LoxFunction(Function<void> *_decl, Environment *_closure, bool _is_initializer) :
    decl{_decl},
    closure{_closure},
    is_initializer{_is_initializer}
    {}

Object LoxFunction::call(Interpreter* intp, std::vector<Object> arguments)
{
    Environment* env {intp->create_environment(closure)};
    for(std::size_t i=0; i<decl->params.size(); i++){
        env->define_name(decl->params[i].get_lexeme(), arguments[i]);
    }
    try{
        intp->execute_block(decl->body, *env);
    }
    catch(ReturnValue& rv){
        return is_initializer ? closure->get_at(0, "this") : rv.value;
    }
    return nullptr;
}

std::size_t LoxFunction::arity()
{
    return decl->params.size();
}

std::string LoxFunction::to_string()
{
    return "<fn " + decl->name.get_lexeme() + ">";
}

LoxFunction LoxFunction::bind(Interpreter* intp, LoxInstance& instance)
{
    Environment* environment {intp->create_environment(closure)};
    environment->define_name("this", instance);
    return LoxFunction(decl, environment, is_initializer);
}
