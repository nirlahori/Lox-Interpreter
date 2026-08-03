#include "token.hpp"

Token::Token(TokenType _type, std::string_view _lexeme, Object _literal, const int &_line) :
    type {_type},
    lexeme {_lexeme},
    literal {std::move(_literal)},
    line {_line}
    {}


std::ostream& operator<< (std::ostream& os, const Token& tok){

    os << "Token: {Lexeme: " << tok.lexeme << ", Line: " << tok.line << ", Value: " << (tok.literal ? std::string(tok.literal).c_str() : "-") << "}";
    return os;
}
