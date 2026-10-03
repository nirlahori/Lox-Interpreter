#ifndef STMT_HPP
#define STMT_HPP

#include <memory>
#include <vector>
#include <map>
#include <typeindex>
#include "expr.hpp"
#include "token.hpp"


template<typename T>
struct Expression;

template<typename T>
struct Print;

template<typename T>
struct Var;

template<typename T>
struct Block;

template<typename T>
struct If;

template<typename T>
struct While;

template<typename T>
struct Break;


template<typename T>
struct Stmt{
    struct Visitor{
        virtual T visit(Expression<T>*) = 0;
        virtual T visit(Print<T>*) = 0;
        virtual T visit(Var<T>*) = 0;
        virtual T visit(Block<T>*) = 0;
        virtual T visit(If<T>*) = 0;
        virtual T visit(While<T>*) = 0;
	virtual T visit(Break<T>*) = 0;
    };

    Stmt() = default;

    virtual T accept(Stmt<T>::Visitor*) = 0;
    virtual ~Stmt() = default;

};


template<typename T>
struct Expression : Stmt<T>{

    std::unique_ptr<Expr<Object>> expr;

    Expression() = default;

    Expression(std::unique_ptr<Expr<Object>> _expr) :
        expr{std::move(_expr)}
        {}

    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};


template<typename T>
struct Print : Stmt<T>{

    std::unique_ptr<Expr<Object>> expr;

    Print() = default;

    Print(std::unique_ptr<Expr<Object>> _expr) :
        expr{std::move(_expr)}
    {}

    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};


template <typename T>
struct Var : Stmt<T>
{
    Token name;
    std::unique_ptr<Expr<Object>> initializer;

    Var() = default;
    Var(Token _name, std::unique_ptr<Expr<Object>> _expr) :
        name{_name},
        initializer{std::move(_expr)}
        {}

    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};



template <typename T>
struct Block : Stmt<T>
{

    std::vector<std::unique_ptr<Stmt<void>>> statements;

    Block() = default;
    Block(std::vector<std::unique_ptr<Stmt<void>>> _statements) :
        statements{std::move(_statements)}
        {}

    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};



template <typename T>
struct If : Stmt<T>
{
    std::unique_ptr<Expr<Object>> condition;
    std::unique_ptr<Stmt<void>> then_branch;
    std::unique_ptr<Stmt<void>> else_branch;

    If() = default;

    If(std::unique_ptr<Expr<Object>> _condition, std::unique_ptr<Stmt<void>> _then_branch, std::unique_ptr<Stmt<void>> _else_branch) :
        condition{std::move(_condition)},
        then_branch{std::move(_then_branch)},
        else_branch{std::move(_else_branch)}
        {}

    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};


template <typename T>
struct While : Stmt<void>
{

    std::unique_ptr<Expr<Object>> condition;
    std::unique_ptr<Stmt<void>> body;

    While() = default;
    While(std::unique_ptr<Expr<Object>> _condition, std::unique_ptr<Stmt<void>> _body) :
        condition{std::move(_condition)},
        body{std::move(_body)}
        {}

    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};

template <typename T>
struct Break : Stmt<void>
{
    Break() = default;
    T accept(typename Stmt<T>::Visitor* visitor){
        visitor->visit(this);
    }
};


static std::map<std::type_index, std::string> stmt_table {
    {std::type_index(typeid(Break<void>)), "Break<void>"}
};

#endif // STMT_HPP
