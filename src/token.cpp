#include "token.hpp"

Token::Token(TokenType _type, std::string_view _lexeme, std::optional<std::string> _literal, const int &_line) :
    type {_type},
    lexeme {_lexeme},
    literal {_literal},
    line {_line}
    {}

void Token::print() const{
    std::printf("Token: {Lexeme: %s, Line: %d, ", lexeme.c_str(), line);
    if(type == TokenType::STRING){
        std::string str = literal.value_or("");
        std::printf("Instance Value: %s }\n", str.c_str());
    }
    else if(type == TokenType::NUMBER){
        double d = std::stod(literal.value_or("0.00"));
        std::printf("Instance Value: %f }\n", d);
    }
    else{
        std::printf("}\n");
    }
}
