#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include "loxcallable.hpp"
#include "loxfunction.hpp"
#include "returnvalue.hpp"

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

Object Interpreter::visit(Logical<Object> *log)
{
    Object left {evaluate(log->left.get())};

    if(log->opr.get_type() == TokenType::OR){
        if(is_truthy(left)){
            return left;
        }
    }
    else{
        if(!is_truthy(left)){
            return left;
        }
    }
    return evaluate(log->right.get());
}

Object Interpreter::visit(Call<Object> *call)
{
    Object callee {evaluate(call->callee.get())};
    std::vector<Object> arguments;
    for(std::unique_ptr<Expr<Object>>& ptr : call->arguments){
        arguments.push_back(evaluate(ptr.get()));
    }
    LoxCallable* function {callee.as_callable()};
    if(!function){
        throw RuntimeError(call->paren, "Can only call functions and classes\n");
    }
    if(arguments.size() != function->arity()){
        throw RuntimeError(call->paren, "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()));
    }
    return function->call(this, std::move(arguments));
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
    Environment* env {create_environment(environment)};
    execute_block(blk->statements, *env);
}

void Interpreter::visit(If<void> *ifstmt)
{
    Object val {evaluate(ifstmt->condition.get())};
    if(is_truthy(val)){
        execute(ifstmt->then_branch.get());
    }
    else if(ifstmt->else_branch){
        execute(ifstmt->else_branch.get());
    }
}

void Interpreter::visit(While<void> *whilestmt)
{
    while(is_truthy(evaluate(whilestmt->condition.get()))){
        execute(whilestmt->body.get());
    }
}

void Interpreter::visit(Function<void> *fun)
{
    LoxFunction function {fun, environment};
    environment->define_name(fun->name.get_lexeme(), function);
}

void Interpreter::visit(Return<void> *ret)
{
    Object value {nullptr};
    if(ret->value){
        value = evaluate(ret->value.get());
    }
    throw ReturnValue{.value=value};
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

Environment* Interpreter::get_global_environment()
{
    return &globals;
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
        else if(val.underlying_type() == "double"){
            return !(val == static_cast<double>(0));
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
    catch(ReturnValue& e){
        this->environment = previous;
        throw e;
    }
    catch(std::exception& e){
        throw e;
    }
    // If exception gets thrown then the previous environment won't get restored and the program
    // will become ill-formed
    this->environment = previous;
}

Environment* Interpreter::create_environment(Environment* enclosing)
{
    envvec.push_back(std::make_unique<Environment>(enclosing));
    return envvec.back().get();
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
        return !is_truthy(value);
    }
}

Object Interpreter::visit(Grouping<Object>* grp)
{
    return evaluate(grp->expression.get());
}
