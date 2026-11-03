#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include "loxcallable.hpp"
#include "loxfunction.hpp"
#include "returnvalue.hpp"
#include "loxclass.hpp"
#include "loxinstance.hpp"

#include "lox.hpp"
#include <iostream>
#include <utility>

Interpreter::Interpreter() = default;


Object Interpreter::visit(Literal<Object>* lit)
{
    return lit->value;
}

Object Interpreter::visit(Variable<Object> *var)
{
    return look_up_variable(var->name, var);
}

Object Interpreter::visit(Assign<Object> *asgn)
{
    Object value {evaluate(asgn->value.get())};
    int distance {};
    try{
        distance = locals.at(asgn);
    }
    catch(std::out_of_range& ex){
        globals.assign(asgn->name, value);
        return value;
    }
    environment->assign_at(distance, asgn->name, value);
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
        throw RuntimeError(call->paren, "Can only call functions and classes");
    }
    if(arguments.size() != function->arity()){
        throw RuntimeError(call->paren, "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()));
    }
    return function->call(this, std::move(arguments));
}

Object Interpreter::visit(Get<Object> *get)
{
    Object object {evaluate(get->object.get())};
    if(object.underlying_type() == "lox_instance"){
        LoxInstance* instance {object.as_lox_class_instance()};
        return instance->get(this, get->name);
    }
    throw RuntimeError(get->name, "Only instances have properties");
}

Object Interpreter::visit(Set<Object> *set)
{
    Object object {evaluate(set->object.get())};
    if(object.underlying_type() != "lox_instance"){
        throw RuntimeError(set->name, "Only instances have fields");
    }
    Object value {evaluate(set->value.get())};
    LoxInstance* instance {object.as_lox_class_instance()};
    if(instance){
        instance->set(set->name, value);
    }
    return value;
}

Object Interpreter::visit(This<Object> *thisexpr)
{
    return look_up_variable(thisexpr->keyword, thisexpr);
}

Object Interpreter::visit(Super<Object> *superexpr)
{
    int distance {locals.at(superexpr)};
    LoxClass* superclass {static_cast<LoxClass*>(environment->get_reference(distance, "super").as_callable())};
    LoxInstance* object {environment->get_at(distance - 1, "this").as_lox_class_instance()};
    if(superclass->contains_method(superexpr->method.get_lexeme())){
        LoxFunction method {superclass->find_method(superexpr->method.get_lexeme())};
        return method.bind(this, *object);
    }
    throw RuntimeError(superexpr->method, "Undefined property'" + superexpr->method.get_lexeme() + "'.");
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
    LoxFunction function {fun, environment, false};
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

void Interpreter::visit(Class<void> *classstmt)
{
    Object* superptr {};
    if(classstmt->superclass){
        int distance {get_distance(classstmt->superclass.get())};
        if(distance != -1){
            superptr = &environment->get_reference(distance, classstmt->superclass->name.get_lexeme());
        }
        else{
            superptr = &globals.get_reference(classstmt->superclass->name);
        }
        if(superptr && superptr->underlying_type() != "lox_class"){
            throw RuntimeError(classstmt->superclass->name, "Superclass must be class");
        }
    }

    environment->define_name(classstmt->name.get_lexeme(), nullptr);

    Environment* enclosing = environment;
    if(superptr){
        Environment* superclass_environment {create_environment(environment)};
        superclass_environment->define_name("super", *superptr);
        environment = superclass_environment;
    }

    std::unordered_map<std::string, LoxFunction> methods;
    for(const std::unique_ptr<Function<void>>& method : classstmt->methods){
        LoxFunction function {method.get(), environment, method->name.get_lexeme() == "init"};
        methods.insert({method->name.get_lexeme(), function});
    }

    if(superptr){
        environment = enclosing;
    }
    LoxClass klass {classstmt->name.get_lexeme(), superptr ? static_cast<LoxClass*>(superptr->as_callable()) : nullptr, methods};
    environment->assign(classstmt->name, klass);
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
    if(expr){
        return expr->accept(this);
    }
    return nullptr;
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

Object Interpreter::look_up_variable(Token name, Expr<Object>* expr)
{
    int distance {};
    try{
        distance = locals.at(expr);
    }
    catch(std::out_of_range& ex){
        return globals.get(name);
    }
    return environment->get_at(distance, name.get_lexeme());
}

int Interpreter::get_distance(Expr<Object>* expr)
{
    try{
        return locals.at(expr);
    }
    catch(...){
        return -1;
    }
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

void Interpreter::resolve(Expr<Object>* expr, int depth)
{
    locals.insert({reinterpret_cast<Expr<Object>*>(expr), depth});
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
                throw RuntimeError(bin->opr, "Double or String expected");
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
