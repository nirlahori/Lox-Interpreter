#ifndef PARSER_HPP
#define PARSER_HPP

#include <list>
#include <vector>
#include <exception>

#include "tokentype.hpp"
#include "token.hpp"
#include "expr.hpp"
#include "object.hpp"
#include "stmt.hpp"

class Parser
{

    struct ParseError : std::exception{};
    struct ParseContext{
        bool is_break_valid;
    };


    std::list<Token> tokens;
    std::list<Token>::iterator current;
    bool is_loop_body {false};
    bool is_block {false};


    std::unique_ptr<Expr<Object>>              expression();
    std::unique_ptr<Stmt<void>>                statement(ParseContext context);
    std::unique_ptr<Stmt<void>>                print_statement();
    std::unique_ptr<Stmt<void>>                expression_statement();
    std::unique_ptr<Stmt<void>>                declaration(ParseContext context);
    std::unique_ptr<Stmt<void>>                var_declaration();
    std::vector<std::unique_ptr<Stmt<void>>>   block(ParseContext context);
    std::unique_ptr<Stmt<void>>                if_statement(ParseContext context);
    std::unique_ptr<Stmt<void>>                While_statement();
    std::unique_ptr<Stmt<void>>                For_statement();
    std::unique_ptr<Stmt<void>>                Break_statement();
    std::unique_ptr<Expr<Object>>              assignment();
    std::unique_ptr<Expr<Object>>              equality();
    std::unique_ptr<Expr<Object>>              comparison();
    std::unique_ptr<Expr<Object>>              term();
    std::unique_ptr<Expr<Object>>              factor();
    std::unique_ptr<Expr<Object>>              unary();
    std::unique_ptr<Expr<Object>>              primary();
    std::unique_ptr<Expr<Object>>              logical_or();
    std::unique_ptr<Expr<Object>>              logical_and();



    bool match(const std::vector<TokenType>& tokens);
    bool check(TokenType type);
    Token advance();
    Token previous();
    bool is_at_end();
    Token peek();

    Token consume(TokenType type, std::string msg);
    ParseError error(Token type, std::string msg);
    void synchronize();


public:
    Parser();
    Parser(std::list<Token> _tokens);
    std::vector<std::unique_ptr<Stmt<void>>> parse();
};

#endif // PARSER_HPP
