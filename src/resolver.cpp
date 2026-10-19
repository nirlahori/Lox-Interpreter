#include <algorithm>
#include <iterator>
#include "resolver.hpp"
#include "lox.hpp"
#include "stmt.hpp"

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
    scoped_variable_info_table.push_back(std::unordered_map<std::string, variable_stat>{});
    scopes.push_back(std::unordered_map<std::string, bool>{});
}

void Resolver::end_scope()
{
    scoped_variable_info_table.pop_back();
    scopes.pop_back();
}

void Resolver::end_scope(const std::vector<std::unique_ptr<Stmt<void>>>& stmts)
{
    check_unused_declarations(stmts);
    end_scope();
}

void Resolver::declare(Token name)
{
    if(!scopes.empty()){
        std::unordered_map<std::string, bool>& scope {scopes.back()};
        if(scope.contains(name.get_lexeme())){
            Lox::error(name, "Already a variable with this name in this scope\n");
            return;
        }
        scope.insert({name.get_lexeme(), false});
        std::unordered_map<std::string, variable_stat>& current_local_variables {scoped_variable_info_table.back()};
        current_local_variables.insert({name.get_lexeme(), variable_stat{.usage=0}});
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
        std::unordered_map<std::string, variable_stat>& variable_table {scoped_variable_info_table[index]};
        variable_table[name.get_lexeme()].usage++;
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
    end_scope(func->body);
    current_function = enclosing_function;
}

void Resolver::check_unused_declarations(const std::vector<std::unique_ptr<Stmt<void>>>& stmts)
{
    check_local_variables(stmts);
}

void Resolver::check_local_variables(const std::vector<std::unique_ptr<Stmt<void>>>& stmts)
{
    std::unordered_map<std::string, variable_stat>& current_local_variables_table {scoped_variable_info_table.back()};
    for(const auto& stmt : stmts){
        auto ptr = stmt.get();
        if(stmt_table[typeid(*ptr)] == "Var<void>"){
            check_local_variable(current_local_variables_table, static_cast<Var<void>*>(stmt.get()));
        }
    }
}

void Resolver::check_local_variable(const std::unordered_map<std::string, variable_stat>& current_local_variables_table, const Var<void>* stmt)
{
    std::string var_name {stmt->name.get_lexeme()};
    if(current_local_variables_table.contains(var_name) && current_local_variables_table.at(var_name).usage < 1){
        Lox::error(stmt->name, "Unused local variable\n");
    }
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

Object Resolver::visit(Literal<Object>*)
{
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

void Resolver::visit(Block<void>* block)
{
    begin_scope();
    resolve(block->statements);
    end_scope(block->statements);
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
        Lox::error(retstmt->keyword, "Can't return from top-level code");
    }
    if(retstmt->value){
        resolve(retstmt->value);
    }
}

void Resolver::visit(While<void>* whilestmt)
{
    resolve(whilestmt->condition);
    resolve(whilestmt->body);
}

