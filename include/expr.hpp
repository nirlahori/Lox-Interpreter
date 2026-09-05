#ifndef EXPR_HPP
#define EXPR_HPP

#include <unordered_map>
#include <typeindex>
#include <string>

#include "token.hpp"
#include "visitor.hpp"


template<typename T>
struct Expr{

    Expr() = default;
    virtual T accept(Visitor<T>* visitor) = 0;
    virtual ~Expr() = default;
};

template<typename T>
struct Binary : Expr<T>{

    std::unique_ptr<Expr<T>> left;
    Token opr;
    std::unique_ptr<Expr<T>> right;
    Binary() = default;
    Binary(std::unique_ptr<Expr<T>> _left, Token _opr, std::unique_ptr<Expr<T>> _right) :
        left{std::move(_left)},
        opr{_opr},
        right{std::move(_right)}
    {}


    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};

template<typename T>
struct Unary : Expr<T>{

    Token opr;
    std::unique_ptr<Expr<T>> right;

    Unary() = default;

    Unary(Token _opr, std::unique_ptr<Expr<T>> _right) :
        opr{_opr},
        right{std::move(_right)}
    {}

    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};

template<typename T>
struct Grouping : Expr<T>{

    std::unique_ptr<Expr<T>> expression;

    Grouping() = default;
    Grouping(std::unique_ptr<Expr<T>> _expression) :
        expression{std::move(_expression)}
        {}

    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};

template<typename T>
struct Literal : Expr<T>{

    Object value;

    Literal() = default;
    Literal(Object _value) :
        value{std::move(_value)}
    {}

    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};


template <typename T>
struct Variable : Expr<T>{

    Token name;
    Variable() = default;

    Variable(Token _name) :
        name{_name}
        {}

    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};


template <typename T>
struct Assign : Expr<T>{

    Token name;
    std::unique_ptr<Expr<T>> value;


    Assign() = default;

    Assign(Token _name, std::unique_ptr<Expr<T>> _value) :
        name{_name},
        value{std::move(_value)}
        {}

    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};


static std::unordered_map<std::type_index, std::string> expr_table {
    {std::type_index(typeid(Expr<Object>)),     "Expr<Object>"},
    {std::type_index(typeid(Binary<Object>)),   "Binary<Object>"},
    {std::type_index(typeid(Unary<Object>)),    "Unary<Object>"},
    {std::type_index(typeid(Grouping<Object>)), "Grouping<Object>"},
    {std::type_index(typeid(Literal<Object>)),  "Literal<Object>"},
    {std::type_index(typeid(Variable<Object>)), "Variable<Object>"},
    {std::type_index(typeid(Assign<Object>)),   "Assign<Object>"}
};


#endif // EXPR_HPP
