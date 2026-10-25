#include "loxinstance.hpp"
#include "runtimeerror.hpp"

LoxInstance::LoxInstance() = default;

LoxInstance::LoxInstance(LoxClass _klass) :
    klass{_klass}
    {}

LoxInstance::LoxInstance(const LoxInstance &other) :
    klass{other.klass},
    fields{other.fields}
    {}

LoxInstance::LoxInstance(LoxInstance&& other) :
    klass{std::move(other.klass)},
    fields{std::move(other.fields)}
    {
        other.fields.clear();
    }

LoxInstance& LoxInstance::operator = (const LoxInstance& other){
    klass = other.klass;
    fields = other.fields;
    return *this;
}


LoxInstance &LoxInstance::operator =(LoxInstance&& other)
{
    klass = std::move(other.klass);
    fields = std::move(other.fields);
    other.fields.clear();
    return *this;
}


Object LoxInstance::get(Interpreter* intp, Token name)
{
    if(fields.contains(name.get_lexeme())){
        return fields.at(name.get_lexeme());
    }
    if(klass.contains_method(name.get_lexeme())){
        LoxFunction method {klass.find_method(name.get_lexeme())};
        return method.bind(intp, *this);
    }
    throw RuntimeError(name, "Undefined property " + name.get_lexeme());
}

void LoxInstance::set(Token name, Object value)
{
    fields.insert({name.get_lexeme(), value});
}

std::string LoxInstance::to_string()
{
    return klass.to_string() + " instance";
}

void LoxInstance::set_class(LoxClass _klass){
    klass = _klass;
}
