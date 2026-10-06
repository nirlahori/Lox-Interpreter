#include "loxfunction.hpp"
#include "returnvalue.hpp"

LoxFunction::LoxFunction() = default;

LoxFunction::LoxFunction(Function<void> *_decl) :
    decl{_decl}
    {}

LoxFunction::LoxFunction(Function<void>* _decl, Environment* _closure) :
    decl{_decl},
    closure{_closure}
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
        return rv.value;
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

/*
void LoxFunction::set_closure(std::optional<Environment> _closure)
{
    closure = _closure;
}
*/

