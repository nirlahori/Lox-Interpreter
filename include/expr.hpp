#ifndef EXPR_HPP
#define EXPR_HPP


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

#endif // EXPR_HPP
