#include <algorithm>
#include <iterator>
#include "resolver.hpp"
#include "lox.hpp"


Resolver::Resolver() = default;


void Resolver::resolve(const std::vector<std::unique_ptr<Stmt<void>>>& stmts)
{
    for(const auto& stmt : stmts){
        resolve(stmt);
    }
}

void Resolver::resolve(const std::unique_ptr<Stmt<void>>& stmt)
{
    stmt->accept(this);
}

void Resolver::resolve(const std::unique_ptr<Expr<Object>>& expr)
{
    expr->accept(this);
}

void Resolver::begin_scope()
{
    scopes.push_back(std::unordered_map<std::string, bool>{});
}

void Resolver::end_scope()
{
    scopes.pop_back();
}

void Resolver::declare(Token name)
{
    if(!scopes.empty()){
        std::unordered_map<std::string, bool> scope {scopes.back()};
        if(scope.contains(name.get_lexeme())){
            Lox::error(name, "Already a variable with this name in this scope\n");
        }
        scope.insert({name.get_lexeme(), false});
    }
}

void Resolver::define(Token name)
{
    if(!scopes.empty()){
        scopes.back()[name.get_lexeme()] = true;
    }
}

void Resolver::resolve_local(Expr<Object>* expr, Token name)
{
    auto find_name {[&name](const std::unordered_map<std::string, bool>& scope){
        if(scope.find(name.get_lexeme()) != scope.cend()){
            return true;
        }
        return false;
        }
    };

    auto iter = std::find_if(scopes.crbegin(), scopes.crend(), find_name);
    int index {get_index(iter)};
    if(index != -1){
        interpreter->resolve(expr, scopes.size() - 1 - index);
    }
}

int Resolver::get_index(std::vector<std::unordered_map<std::string, bool>>::const_reverse_iterator iter)
{
    auto first {scopes.crbegin()};
    std::size_t index {scopes.size() - 1};
    while(first != scopes.crend()){
        if(first == iter){
            return index;
        }
        first = std::next(first);
        index--;
    }
    return -1;
}

void Resolver::resolve_function(Function<void>* func, FunctionType type)
{
    FunctionType enclosing_function {current_function};
    current_function = type;

    begin_scope();
    for(const Token& param : func->params){
        declare(param);
        define(param);
    }
    resolve(func->body);
    end_scope();
    current_function = enclosing_function;
}

void Resolver::resolve_getter_field(Getter_Field<void>* getter)
{
    FunctionType enclosing_function {current_function};
    current_function = FunctionType::getter_field;
    begin_scope();
    resolve(getter->body);
    end_scope();
    current_function = enclosing_function;
}

Resolver::Resolver(Interpreter* _interpreter) :

    interpreter{_interpreter}
    {}

Object Resolver::visit(Variable<Object>* varexpr)
{
    if(!scopes.empty()){
        auto iter {scopes.back().find(varexpr->name.get_lexeme())};
        if(iter != scopes.back().end()){
            if(!iter->second){
                Lox::error(varexpr->name, "Can't read local variable in its own initializer\n");
            }
        }
    }
    resolve_local(varexpr, varexpr->name);
    return nullptr;
}

Object Resolver::visit(Assign<Object>* assignexpr)
{
    resolve(assignexpr->value);
    resolve_local(assignexpr, assignexpr->name);
    return nullptr;
}

Object Resolver::visit(Binary<Object>* binexpr)
{
    resolve(binexpr->left);
    resolve(binexpr->right);
    return nullptr;
}

Object Resolver::visit(Call<Object>* callexpr)
{
    resolve(callexpr->callee);
    for(const std::unique_ptr<Expr<Object>>& argument : callexpr->arguments){
        resolve(argument);
    }
    return nullptr;
}

Object Resolver::visit(Grouping<Object>* groupingexpr)
{
    resolve(groupingexpr->expression);
    return nullptr;
}

Object Resolver::visit(Logical<Object>* logicalexpr)
{
    resolve(logicalexpr->left);
    resolve(logicalexpr->right);
    return nullptr;
}

Object Resolver::visit(Unary<Object>* unaryexpr)
{
    resolve(unaryexpr->right);
    return nullptr;
}

Object Resolver::visit(Get<Object>* getexpr)
{
    resolve(getexpr->object);
    return nullptr;
}

Object Resolver::visit(Set<Object>* setexpr)
{
    resolve(setexpr->value);
    resolve(setexpr->object);
    return nullptr;
}

Object Resolver::visit(This<Object>* thisexpr)
{
    if(current_class != ClassType::Class){
        Lox::error(thisexpr->keyword, "Can't use 'this' outside of a class\n");
    }
    else if(current_function == FunctionType::static_method){
        Lox::error(thisexpr->keyword, "Can't use 'this' inside static method\n");
    }
    else{
        resolve_local(thisexpr, thisexpr->keyword);
    }
    return nullptr;
}

void Resolver::visit(Block<void>* block)
{
    begin_scope();
    resolve(block->statements);
    end_scope();
}

void Resolver::visit(Var<void>* var)
{
    declare(var->name);
    if(var->initializer){
        resolve(var->initializer);
    }
    define(var->name);
}

void Resolver::visit(Function<void>* func)
{
    declare(func->name);
    define(func->name);
    resolve_function(func, FunctionType::function);
}

void Resolver::visit(Expression<void>* expr)
{
    resolve(expr->expr);
}

void Resolver::visit(If<void>* ifstmt)
{
    resolve(ifstmt->condition);
    resolve(ifstmt->then_branch);
    if(ifstmt->else_branch){
        resolve(ifstmt->else_branch);
    }
}

void Resolver::visit(Print<void>* printstmt)
{
    resolve(printstmt->expr);
}

void Resolver::visit(Return<void>* retstmt)
{
    if(current_function == FunctionType::none){
        Lox::error(retstmt->keyword, "Can't return from top-level code\n");
    }
    if(retstmt->value){
        if(current_function == FunctionType::initializer){
            Lox::error(retstmt->keyword, "Can't return a value from an initializer\n");
        }
        resolve(retstmt->value);
    }
}

void Resolver::visit(While<void>* whilestmt)
{
    resolve(whilestmt->condition);
    resolve(whilestmt->body);
}

void Resolver::visit(Class<void>* classstmt)
{
    ClassType enclosing_class {current_class};
    current_class = ClassType::Class;
    declare(classstmt->name);
    define(classstmt->name);
    begin_scope();
    scopes.back().insert({"this", true});
    for(const std::unique_ptr<Function<void>>& func : classstmt->methods){
        FunctionType declaration {func->name.get_lexeme() == "init" ? FunctionType::initializer : FunctionType::method};
        resolve_function(func.get(), declaration);
    }
    for(const std::unique_ptr<Function<void>>& func : classstmt->static_methods){
        FunctionType declaration {FunctionType::static_method};
        resolve_function(func.get(), declaration);
    }

    for(const std::unique_ptr<Getter_Field<void>>& getterfield : classstmt->getter_fields){
        resolve_getter_field(getterfield.get());
    }

    end_scope();
    current_class = enclosing_class;
}

void Resolver::visit(Getter_Field<void>* getterstmt)
{
    begin_scope();
    resolve(getterstmt->body);
    end_scope();
}
