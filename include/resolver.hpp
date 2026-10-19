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
    function
};


struct variable_stat{
    int usage {};
};

class Resolver : public Expr<Object>::Visitor, public Stmt<void>::Visitor
{
    std::vector<std::unordered_map<std::string, bool>> scopes;
    std::vector<std::unordered_map<std::string, variable_stat>> scoped_variable_info_table;

    Interpreter* interpreter;
    FunctionType current_function {FunctionType::none};

    void resolve(const std::unique_ptr<Stmt<void>>&);
    void resolve(const std::unique_ptr<Expr<Object>>&);

    void begin_scope();
    void end_scope();
    void end_scope(const std::vector<std::unique_ptr<Stmt<void>>>&);
    void declare(Token name);
    void define(Token name);



    void resolve_local(Expr<Object>*, Token);
    int  get_index(std::vector<std::unordered_map<std::string, bool>>::const_reverse_iterator);
    void resolve_function(Function<void>*, FunctionType);

    void check_unused_declarations(const std::vector<std::unique_ptr<Stmt<void>>>&);
    void check_local_variables(const std::vector<std::unique_ptr<Stmt<void>>>&);
    void check_local_variable(const std::unordered_map<std::string, variable_stat>&, const Var<void>*);

public:

    Resolver();
    Resolver(Interpreter*);

    void resolve(const std::vector<std::unique_ptr<Stmt<void>>>&);
    Object visit(Variable<Object>*);
    Object visit(Assign<Object>*);
    Object visit(Binary<Object>*);
    Object visit(Call<Object>*);
    Object visit(Grouping<Object>*);
    Object visit(Literal<Object>*);
    Object visit(Logical<Object>*);
    Object visit(Unary<Object>*);


    void visit(Block<void>*);
    void visit(Var<void>*);
    void visit(Function<void>*);
    void visit(Expression<void>*);
    void visit(If<void>*);
    void visit(Print<void>*);
    void visit(Return<void>*);
    void visit(While<void>*);
};

#endif // RESOLVER_HPP
