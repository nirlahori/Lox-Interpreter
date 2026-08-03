#ifndef SCANNER_HPP
#define SCANNER_HPP


#include "token.hpp"
#include "object.hpp"

#include <string>
#include <string_view>
#include <list>
#include <map>

class Scanner
{

    std::string source;
    std::list<Token> tokens;

    std::size_t start {};
    std::size_t current {};
    int line {};

    static std::map<std::string, TokenType> keywords;

    bool is_at_end() const;
    void scan_token();
    char advance();
    void add_token(TokenType type);
    void add_token(TokenType type, Object lit);
    bool match(char current_ch);
    char peek() const;
    char peek_next() const;
    void expect_string();
    void expect_number();
    void expect_identifier();
    void expect_comment();

public:
    Scanner() = default;
    Scanner(std::string_view _source);
    std::list<Token> scan_tokens();


};

#endif // SCANNER_HPP
