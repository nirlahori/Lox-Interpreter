#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include "lox.hpp"
#include <iostream>

Interpreter::Interpreter() = default;


Object Interpreter::visit(Literal<Object>* lit)
{
    return lit->value;
}

void Interpreter::interpret(Expr<Object> *expr)
{
    try{
        Object value = evaluate(expr);
        std::cout << value << std::endl;
    }
    catch(RuntimeError error){
        Lox::runtime_error(error);
    }
}

Object Interpreter::evaluate(Expr<Object> *expr)
{
    return expr->accept(this);
}

bool Interpreter::is_truthy(Object val)
{
    if(val){
        if(val.underlying_type() == "bool"){
            return static_cast<bool>(val);
        }
        return true;
    }
    return false;
}

bool Interpreter::is_equal(const Object &left, const Object &right)
{
    if(!left && !right){
        return true;
    }
    else if(!left){
        return false;
    }
    return left == right;
}

void Interpreter::check_number_operand(Token opr, const Object &operand)
{
    if(operand.underlying_type() != "double"){
        throw RuntimeError(opr, "Operand must be a number");
    }
}

void Interpreter::check_number_operand(Token opr, const Object &left, const Object &right)
{
    if(left.underlying_type() != "double" || right.underlying_type() != "double"){
        throw RuntimeError(opr, "Operands must be numbers");
    }
}

std::string Interpreter::stringify(Object obj)
{
    if(!obj){
        return "NULL";
    }
    else if(obj.underlying_type() == "double") {
        std::string text {obj};
        std::size_t pos {text.find('.')};
        text.erase(pos);
        return text;
    }
    return obj;
}

void Interpreter::check_division_by_zero(Token opr, const Object& value)
{
    if(std::stod(value) == 0){
        throw RuntimeError(opr, "Attempt to divide value by zero");
    }
}

Object Interpreter::visit(Binary<Object>* bin)
{
    Object left {evaluate(bin->left.get())};
    Object right {evaluate(bin->right.get())};

    switch(bin->opr.get_type()){
        case TokenType::GREATER:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) > std::stod(right);
        case TokenType::GREATER_EQUAL:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) >= std::stod(right);
        case TokenType::LESS:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) < std::stod(right);
        case TokenType::LESS_EQUAL:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) <= std::stod(right);
        case TokenType::MINUS:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) - std::stod(right);
        case TokenType::SLASH:
            check_number_operand(bin->opr, left, right);
            check_division_by_zero(bin->opr, right);
            return std::stod(left) / std::stod(right);
        case TokenType::STAR:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) * std::stod(right);
        case TokenType::PLUS:
            if(left.underlying_type() == "double" && right.underlying_type() == "double"){
                return std::stod(left) + std::stod(right);
            }
            else if(left.underlying_type() == "string" || right.underlying_type() == "string"){
                return static_cast<std::string>(left) + static_cast<std::string>(right);
            }
            else{
                throw RuntimeError(bin->opr, "Adding unexpected types\n");
            }
        case TokenType::BANG_EQUAL:
            return !is_equal(left, right);
        case TokenType::EQUAL_EQUAL:
            return is_equal(left, right);
        default:
            return nullptr;
    }
}


Object Interpreter::visit(Unary<Object>* unry)
{
    Object value {evaluate(unry->right.get())};

    if(unry->opr.get_type() == TokenType::MINUS){
        check_number_operand(unry->opr, value);
        return -static_cast<double>(value);
    }
    else{
        return is_truthy(value);
    }
}

Object Interpreter::visit(Grouping<Object>* grp)
{
    return evaluate(grp->expression.get());
}
