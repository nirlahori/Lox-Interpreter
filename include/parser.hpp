#ifndef PARSER_HPP
#define PARSER_HPP

#include <list>
#include <vector>
#include <exception>

#include "tokentype.hpp"
#include "token.hpp"
#include "expr.hpp"
#include "object.hpp"

class Parser
{

    struct ParseError : std::exception{};


    std::list<Token> tokens;
    std::list<Token>::iterator current;


    std::unique_ptr<Expr<Object>> expression();
    std::unique_ptr<Expr<Object>> equality();
    std::unique_ptr<Expr<Object>> comparison();
    std::unique_ptr<Expr<Object>> term();
    std::unique_ptr<Expr<Object>> factor();
    std::unique_ptr<Expr<Object>> unary();
    std::unique_ptr<Expr<Object>> primary();



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
    std::unique_ptr<Expr<Object>> parse();
};

#endif // PARSER_HPP
