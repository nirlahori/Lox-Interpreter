#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <vector>
#include <memory>

#include "object.hpp"
#include "visitor.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"

class Interpreter : public Visitor<Object>, public Stmt<void>::Visitor
{

    Object evaluate(Expr<Object>* expr);
    bool is_truthy(Object val);
    bool is_equal(const Object& left, const Object& right);
    void check_number_operand(Token opr, const Object& operand);
    void check_number_operand(Token opr, const Object& left, const Object& right);
    std::string stringify(Object obj);

    void execute(Stmt<void>* stmt);


    Environment globals;
    Environment* environment {&globals};

    std::vector<std::unique_ptr<Environment>> envvec;


public:
    Interpreter();
    Interpreter(Environment* env);

    Object visit(Binary<Object>* bin);
    Object visit(Unary<Object>* unry);
    Object visit(Grouping<Object>* grp);
    Object visit(Literal<Object>* lit);
    Object visit(Variable<Object>* var);
    Object visit(Assign<Object>* asgn);
    Object visit(Logical<Object>* log);
    Object visit(Call<Object>* call);
    Object visit(Lambda<Object>* lam);

    void visit(Expression<void>* expr);
    void visit(Print<void>* prt);
    void visit(Var<void>* var);
    void visit(Block<void>* blk);
    void visit(If<void>* ifstmt);
    void visit(While<void>* whilestmt);
    void visit(Function<void>* fun);
    void visit(Return<void>* ret);


    void interpret(std::vector<std::unique_ptr<Stmt<void>>> statements);
    Environment* get_global_environment();
    void execute_block(const std::vector<std::unique_ptr<Stmt<void>>>&, Environment&);
    Environment* create_environment(Environment*);

};

#endif // INTERPRETER_HPP
