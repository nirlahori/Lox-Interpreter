#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP


#include "object.hpp"
#include "visitor.hpp"
#include "expr.hpp"
#include "tokentype.hpp"

class Interpreter : public Visitor<Object>
{

    Object evaluate(Expr<Object>* expr);
    bool is_truthy(Object val);
    bool is_equal(const Object& left, const Object& right);
    void check_number_operand(Token opr, const Object& operand);
    void check_number_operand(Token opr, const Object& left, const Object& right);
    std::string stringify(Object obj);

    void check_division_by_zero(Token opr, const Object& value);

public:
    Interpreter();

    Object visit(Binary<Object>* bin);
    Object visit(Unary<Object>* unry);
    Object visit(Grouping<Object>* grp);
    Object visit(Literal<Object>* lit);

    void interpret(Expr<Object>* expr);


};

#endif // INTERPRETER_HPP
