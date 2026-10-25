#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

#include "expr.hpp"
#include "stmt.hpp"
#include "interpreter.hpp"
#include "token.hpp"


enum class FunctionType{
    none,
    function,
    method,
    initializer
};

enum class ClassType{
    none,
    Class
};



class Resolver : public Expr<Object>::Visitor, public Stmt<void>::Visitor
{
    std::vector<std::unordered_map<std::string, bool>> scopes;

    Interpreter* interpreter;
    FunctionType current_function {FunctionType::none};
    ClassType current_class {ClassType::none};

    void resolve(const std::unique_ptr<Stmt<void>>&);
    void resolve(const std::unique_ptr<Expr<Object>>&);

    void begin_scope();
    void end_scope();
    void declare(Token name);
    void define(Token name);
    void resolve_local(Expr<Object>*, Token);
    int get_index(std::vector<std::unordered_map<std::string, bool>>::const_reverse_iterator);
    void resolve_function(Function<void>*, FunctionType);


public:

    Resolver();
    Resolver(Interpreter*);

    void resolve(const std::vector<std::unique_ptr<Stmt<void>>>&);
    Object visit(Variable<Object>*);
    Object visit(Assign<Object>*);
    Object visit(Binary<Object>*);
    Object visit(Call<Object>*);
    Object visit(Grouping<Object>*);
    Object visit(Literal<Object>*){
        return nullptr;
    }
    Object visit(Logical<Object>*);
    Object visit(Unary<Object>*);
    Object visit(Get<Object>*);
    Object visit(Set<Object>*);
    Object visit(This<Object>*);


    void visit(Block<void>*);
    void visit(Var<void>*);
    void visit(Function<void>*);
    void visit(Expression<void>*);
    void visit(If<void>*);
    void visit(Print<void>*);
    void visit(Return<void>*);
    void visit(While<void>*);
    void visit(Class<void>*);
};

#endif // RESOLVER_HPP
