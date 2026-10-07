#include "loxlambda.hpp"
#include "returnvalue.hpp"

LoxLambda::LoxLambda() = default;

LoxLambda::LoxLambda(Lambda<Object>* _func, Environment* _closure) :
    func{_func},
    closure{_closure}
{}

Object LoxLambda::call(Interpreter* intp, std::vector<Object> arguments)
{
    Environment* env {intp->create_environment(closure)};
    for(std::size_t i=0; i<func->params.size(); i++){
        env->define_name(func->params[i].get_lexeme(), arguments[i]);
    }
    try{
        intp->execute_block(func->body, *env);
    }
    catch(ReturnValue& rv){
        return rv.value;
    }
    return nullptr;
}

std::size_t LoxLambda::arity()
{
    return func->params.size();
}

std::string LoxLambda::to_string()
{
    return "<lambda>";
}
