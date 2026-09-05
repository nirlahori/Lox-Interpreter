#ifndef VISITOR_HPP
#define VISITOR_HPP

template<typename T>
struct Binary;

template<typename T>
struct Unary;

template<typename T>
struct Grouping;

template<typename T>
struct Literal;

template<typename T>
struct Variable;

template<typename T>
struct Assign;


template<typename T>
struct Visitor{

    virtual T visit(Binary<T>*) = 0;
    virtual T visit(Unary<T>*) = 0;
    virtual T visit(Grouping<T>*) = 0;
    virtual T visit(Literal<T>*) = 0;
    virtual T visit(Variable<T>*) = 0;
    virtual T visit(Assign<T>*) = 0;
};

#endif // VISITOR_HPP
