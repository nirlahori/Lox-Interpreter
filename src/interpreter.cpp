#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include "lox.hpp"
#include <iostream>

Interpreter::Interpreter() = default;


Object Interpreter::visit(Literal<Object>* lit)
{
    return lit->value;
}

Object Interpreter::visit(Variable<Object> *var)
{
    return environment->get(var->name);
}

Object Interpreter::visit(Assign<Object> *asgn)
{
    Object value {evaluate(asgn->value.get())};
    environment->assign(asgn->name, value);
    return value;
}

void Interpreter::visit(Expression<void> *stmt)
{
    evaluate(stmt->expr.get());
}

void Interpreter::visit(Print<void> *prt)
{
    Object value = evaluate(prt->expr.get());
    std::cout << value << std::endl;
}

void Interpreter::visit(Var<void> *var)
{
    Object value (nullptr);
    if(var->initializer){
        value = evaluate(var->initializer.get());
    }
    environment->define_name(var->name.get_lexeme(), value);
}

void Interpreter::visit(Block<void> *blk)
{

    Environment env (environment);
    execute_block(blk->statements, env);
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt<void>>> statements)
{
    try {
        for(std::unique_ptr<Stmt<void>>& stmt : statements){
            if(stmt){
                execute(stmt.get());
            }
        }

    } catch (RuntimeError& error) {
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
        throw RuntimeError(opr, "Operand must be a number\n");
    }
}

void Interpreter::check_number_operand(Token opr, const Object &left, const Object &right)
{
    if(left.underlying_type() != "double" || right.underlying_type() != "double"){
        throw RuntimeError(opr, "Operands must be numbers\n");
    }
}

std::string Interpreter::stringify(Object obj)
{
    if(!obj){
        return "NULL";
    }
    else if(obj.underlying_type() == "double") {
        //double data {static_cast<double>(obj)};
        // std::size_t pos {text.find('.')};
        // if(pos != std::string::npos){
        //     text.erase(pos);
        // }
        return std::to_string(obj);
    }
    return obj;
}

void Interpreter::execute(Stmt<void>* stmt)
{
    stmt->accept(this);
}

void Interpreter::execute_block(const std::vector<std::unique_ptr<Stmt<void>>>& statements, Environment& env)
{
    Environment* previous = environment;
    try{
        this->environment = &env;
        for(const std::unique_ptr<Stmt<void>>& stmt : statements){
            if(stmt){
                execute(stmt.get());
            }
        }
    }
    catch(std::exception& e){
        throw e;
    }
    // If exception gets thrown then the previous environment won't get restored and the program
    // will become ill-formed
    this->environment = previous;
}

Interpreter::Interpreter(Environment *env) :
    environment{env}
    {}

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
            return std::stod(left) / std::stod(right);
        case TokenType::STAR:
            check_number_operand(bin->opr, left, right);
            return std::stod(left) * std::stod(right);
        case TokenType::PLUS:
            if(left.underlying_type() == "double" && right.underlying_type() == "double"){
                return std::stod(left) + std::stod(right);
            }
            else if(left.underlying_type() == "string" && right.underlying_type() == "string"){
                return static_cast<std::string>(left) + static_cast<std::string>(right);
            }
            else{
                throw RuntimeError(bin->opr, "Double or String expected\n");
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
