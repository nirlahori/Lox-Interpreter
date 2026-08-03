#ifndef EXPR_HPP
#define EXPR_HPP


#include "token.hpp"
#include "visitor.hpp"

template<typename T>
struct Expr{

    Expr() = default;
    virtual T accept(Visitor<T>* visitor) = 0;
};

template<typename T>
struct Binary : Expr<T>{

    Expr<T>* left;
    Token opr;
    Expr<T>* right;
    Binary() = default;
    Binary(Expr<T>* _left, Token _opr, Expr<T>* _right) :
        left{_left},
        opr{_opr},
        right{_right}
    {}


    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};

template<typename T>
struct Unary : Expr<T>{

    Token opr;
    Expr<T>* right;

    Unary() = default;

    Unary(Token _opr, Expr<T>* _right) :
        opr{_opr},
        right{_right}
    {}

    T accept(Visitor<T>* visitor){
        return visitor->visit(this);
    }
};

template<typename T>
struct Grouping : Expr<T>{

    Expr<T>* expression;

    Grouping() = default;
    Grouping(Expr<T>* _expression) :
        expression{_expression}
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

#endif // EXPR_HPP
