#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <cstdio>
#include <optional>

#include "tokentype.hpp"



class Token
{

    TokenType type;
    std::string lexeme;
    std::optional<std::string> literal;
    int line;


public:
    Token(TokenType _type, std::string_view _lexeme, std::optional<std::string> _literal, const int& _line);
    void print() const;
};

#endif // TOKEN_HPP
