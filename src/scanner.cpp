#include "scanner.hpp"
#include "lox.hpp"

#include <list>
#include <cctype>

bool Scanner::is_at_end() const{
    return current >= source.length();
}

void Scanner::scan_token(){

    char ch {advance()};
    switch(ch){
        case '(':
            add_token(TokenType::LEFT_PAREN);
            break;
        case ')':
            add_token(TokenType::RIGHT_PAREN);
            break;
        case '{':
            add_token(TokenType::LEFT_BRACE);
            break;
        case '}':
            add_token(TokenType::RIGHT_BRACE);
            break;
        case ',':
            add_token(TokenType::COMMA);
            break;
        case '.':
            add_token(TokenType::DOT);
            break;
        case '-':
            add_token(TokenType::MINUS);
            break;
        case '+':
            add_token(TokenType::PLUS);
            break;
        case ';':
            add_token(TokenType::SEMICOLON);
            break;
        case '*':
            add_token(TokenType::STAR);
            break;
        case '!':
            add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '>':
            add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
        case '<':
            add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '/':
            if(match('/')){
                while(peek() != '\n' && !is_at_end()){
                    advance();
                }
            }
            else{
                add_token(TokenType::SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;

        case '"':
            expect_string();
            break;
        default:
            if(std::isdigit(ch)){
                expect_number();
            }
            else if(std::isalpha(ch)){
                expect_identifier();
            }
            else{
                Lox::error(line, "Unexpected character");
            }
    }
}

char Scanner::advance(){
    char next_char {source.at(current++)};
    return next_char;
}

void Scanner::add_token(TokenType type){
    add_token(type, std::nullopt);
}

void Scanner::add_token(TokenType type, std::optional<std::string> lit){
    std::string lex {source.substr(start, current - start)};
    Token tok(type, lex, lit, line);
    tokens.push_back(tok);
}

bool Scanner::match(char current_ch){

    if(is_at_end()){
        return false;
    }
    if(source.at(current) == current_ch){
        current++;
        return true;
    }
    return false;
}

char Scanner::peek() const{

    if(is_at_end()){
        return '\0';
    }
    return source.at(current);
}

char Scanner::peek_next() const{
    if(current + 1 >= source.length()){
        return '\0';
    }
    return source[current + 1];
}

void Scanner::expect_string(){

    char ch;
    while((ch = peek()) != '"' && !is_at_end()){
        if(ch == '\n'){
            line++;
        }
        advance();
    }

    if(!is_at_end()){
        std::string str_value {source.substr(start + 1, current - (start + 1))};
        advance(); // Closing " (double quote)
        add_token(TokenType::STRING, str_value);
    }
    else{
        Lox::error(line, "Unterminated string\n");
    }
}

void Scanner::expect_number(){

    while(std::isdigit(peek())){
        advance();
    }

    // Look for a fractional part
    if(peek() == '.' && std::isdigit(peek_next())){
        advance();
        while(std::isdigit(peek())){
            advance();
        }
    }

    double d {std::stod(source.substr(start, current - start))};
    add_token(TokenType::NUMBER, std::to_string(d));
}

void Scanner::expect_identifier(){

    char ch;
    while((ch = peek()) != ' ' && !is_at_end()){
        if(std::isalnum(ch) || ch == '_'){
            advance();
        }
        else{
            break;
        }
    }

    std::string str {source.substr(start, current - start)};
    std::printf("Keyword: %s\n", str.c_str());
    if(keywords.find(str) != keywords.end()){
        add_token(keywords.at(str));
    }
    else{
        add_token(TokenType::IDENTIFIER);
    }
}


Scanner::Scanner(std::string_view _source) :
    source{_source},
    tokens{},
    start{0},
    current{0},
    line{1} {}

std::list<Token> Scanner::scan_tokens(){

    while(!is_at_end()){
        start = current;
        scan_token();
    }
    Token tok(TokenType::END_OF_FILE, "", std::nullopt, line);
    tokens.push_back(tok);
    return tokens;
}


std::map<std::string, TokenType> Scanner::keywords{
    {"and",    TokenType::AND},
    {"class",  TokenType::CLASS},
    {"else",   TokenType::ELSE},
    {"false",  TokenType::FALSE},
    {"for",    TokenType::FOR},
    {"fun",    TokenType::FUN},
    {"if",     TokenType::IF},
    {"nil",    TokenType::NIL},
    {"or",     TokenType::OR},
    {"print",  TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super",  TokenType::SUPER},
    {"this",   TokenType::THIS},
    {"true",   TokenType::TRUE},
    {"var",    TokenType::VAR},
    {"while",  TokenType::WHILE}
};

