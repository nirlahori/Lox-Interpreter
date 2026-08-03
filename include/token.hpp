#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <cstdio>
#include <ostream>

#include "tokentype.hpp"
#include "object.hpp"


class Token
{

    TokenType type;
    std::string lexeme;
    Object literal;
    int line;


public:

    Token() = default;
    Token(TokenType _type, std::string_view _lexeme, Object _literal, const int& _line);

    std::string get_lexeme(){
        return lexeme;
    }

    const Object& get_value(){
        return literal;
    }

    friend std::ostream& operator<< (std::ostream& os, const Token& tok);
};

#endif // TOKEN_HPP
