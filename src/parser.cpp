#include <iterator>
#include <memory>

#include "parser.hpp"
#include "lox.hpp"

Parser::Parser() = default;


std::unique_ptr<Expr<Object>> Parser::expression()
{
    return assignment();
}

std::unique_ptr<Stmt<void>> Parser::statement(ParseContext context)
{
    if(match({TokenType::PRINT})){
        return print_statement();
    }
    if(match({TokenType::LEFT_BRACE})){
        return std::make_unique<Block<void>>(block(context));
    }
    if(match({TokenType::IF})){
        return if_statement(context);
    }
    if(match({TokenType::WHILE})){
        return While_statement();
    }
    if(match({TokenType::FOR})){
        return For_statement();
    }
    if(match({TokenType::BREAK})){
        if(context.is_break_valid){
            return Break_statement();
        }
        error(previous(), "break statement used outside of loop body\n");
    }
    return expression_statement();
}

std::unique_ptr<Stmt<void>> Parser::if_statement(ParseContext context){
    consume(TokenType::LEFT_PAREN, "Expect '(' after if");
    std::unique_ptr<Expr<Object>> condition {expression()};
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");
    std::unique_ptr<Stmt<void>> then_branch {statement(context)};
    std::unique_ptr<Stmt<void>> else_branch {};
    if(match({TokenType::ELSE})){
        else_branch = statement(context);
    }
    return std::make_unique<If<void>>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Stmt<void>> Parser::While_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after while.");
    std::unique_ptr<Expr<Object>> condition {expression()};
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

    if(check(TokenType::BREAK)){
        error(peek(), "break statement used outside of loop body\n");
    }

    std::unique_ptr<Stmt<void>> body {statement(ParseContext{.is_break_valid=true})};
    return std::make_unique<While<void>>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt<void>> Parser::For_statement()
{

    consume(TokenType::LEFT_PAREN, "Expect '(' after for.");
    std::unique_ptr<Stmt<void>> init_stmt;

    if(match({TokenType::SEMICOLON})){
        init_stmt = nullptr;
    }
    else if(match({TokenType::VAR})){
        init_stmt = var_declaration();
    }
    else{
        init_stmt = expression_statement();
    }

    std::unique_ptr<Expr<Object>> cond {};
    if(!check(TokenType::SEMICOLON)){
        cond = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition");

    std::unique_ptr<Expr<Object>> incr;
    if(!check(TokenType::RIGHT_PAREN)){
        incr = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses");


    std::unique_ptr<Stmt<void>> body {statement(ParseContext{.is_break_valid=true})};

    if(incr){
        std::vector<std::unique_ptr<Stmt<void>>> loop_body;
        loop_body.push_back(std::move(body));
        loop_body.push_back(std::make_unique<Expression<void>>(std::move(incr)));
        body = std::make_unique<Block<void>>(std::move(loop_body));
    }

    if(!cond){
        cond = std::make_unique<Literal<Object>>(true);
    }
    body = std::make_unique<While<void>>(std::move(cond), std::move(body));

    if(init_stmt){
        std::vector<std::unique_ptr<Stmt<void>>> init_body;
        init_body.push_back(std::move(init_stmt));
        init_body.push_back(std::move(body));
        body = std::make_unique<Block<void>>(std::move(init_body));
    }
    return body;
}

std::unique_ptr<Stmt<void>> Parser::Break_statement()
{
    consume(TokenType::SEMICOLON, "Expect ';' after break\n");
    return std::make_unique<Break<void>>();
}

std::unique_ptr<Stmt<void>> Parser::print_statement()
{
    std::unique_ptr<Expr<Object>> value {expression()};
    consume(TokenType::SEMICOLON, "Expect ';' after value\n");
    return std::make_unique<Print<void>>(std::move(value));
}

std::unique_ptr<Stmt<void>> Parser::expression_statement()
{
    std::unique_ptr<Expr<Object>> value {expression()};
    consume(TokenType::SEMICOLON, "Expect ';' after value\n");
    return std::make_unique<Expression<void>>(std::move(value));
}

std::unique_ptr<Stmt<void>> Parser::declaration(ParseContext context)
{
    try {

        if(match({TokenType::VAR})){
            return var_declaration();
        }
        return statement(context);

    } catch (ParseError& error) {
        synchronize();
        // This will return nullptr as a next statement which will cause a segfault.
        // Identify the appropriate course of action
        return nullptr;
    }
}

std::unique_ptr<Stmt<void>> Parser::var_declaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name\n");
    std::unique_ptr<Expr<Object>> initializer {nullptr};

    if(match({TokenType::EQUAL})){
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration\n");
    return std::make_unique<Var<void>>(name, std::move(initializer));
}

std::vector<std::unique_ptr<Stmt<void>>> Parser::block(ParseContext context)
{
    std::vector<std::unique_ptr<Stmt<void>>> statements;
    while(!check(TokenType::RIGHT_BRACE) && !is_at_end()){
        statements.push_back(declaration(context));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block\n");
    return statements;

}

std::unique_ptr<Expr<Object>> Parser::assignment()
{
    std::unique_ptr<Expr<Object>> expr {logical_or()};
    auto ptr = expr.get();

    if(match({TokenType::EQUAL})){
        Token equals = previous();
        std::unique_ptr<Expr<Object>> value {assignment()};
        if(expr_table[typeid(*ptr)] == "Variable<Object>"){
            Token name = (static_cast<Variable<Object>*>(expr.get()))->name;
            return std::make_unique<Assign<Object>>(name, std::move(value));
        }
        error(equals, "Invalid assignment target\n");
    }
    return expr;
}

std::unique_ptr<Expr<Object>> Parser::equality()
{
    std::unique_ptr<Expr<Object>> expr {comparison()};
    while(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})){
        Token opr = previous();
        std::unique_ptr<Expr<Object>> right = comparison();
        expr = std::make_unique<Binary<Object>>(std::move(expr), opr, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr<Object>> Parser::comparison()
{
    std::unique_ptr<Expr<Object>> expr {term()};
    while(match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})){
        Token opr = previous();
        std::unique_ptr<Expr<Object>> right = term();
        expr = std::make_unique<Binary<Object>>(std::move(expr), opr, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr<Object>> Parser::term()
{
    std::unique_ptr<Expr<Object>> expr = factor();
    while(match({TokenType::MINUS, TokenType::PLUS})){
        Token opr = previous();
        std::unique_ptr<Expr<Object>> right = factor();
        expr = std::make_unique<Binary<Object>>(std::move(expr), opr, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr<Object>> Parser::factor()
{
    std::unique_ptr<Expr<Object>> expr = unary();
    while(match({TokenType::SLASH, TokenType::STAR})){
        Token opr = previous();
        std::unique_ptr<Expr<Object>> right = unary();
        expr = std::make_unique<Binary<Object>>(std::move(expr), opr, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr<Object>> Parser::unary()
{
    if(match({TokenType::MINUS, TokenType::BANG})){
        Token opr = previous();
        std::unique_ptr<Expr<Object>> right = unary();
        return std::make_unique<Unary<Object>>(opr, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr<Object>> Parser::primary()
{
    if(match({TokenType::FALSE})){
        return std::make_unique<Literal<Object>>(false);
    }
    else if(match({TokenType::TRUE})){
        return std::make_unique<Literal<Object>>(true);
    }
    else if(match({TokenType::NIL})){
        return std::make_unique<Literal<Object>>(nullptr);
    }

    if(match({TokenType::NUMBER, TokenType::STRING})){
        return std::make_unique<Literal<Object>>(previous().get_value());
    }

    if(match({TokenType::LEFT_PAREN})){
        std::unique_ptr<Expr<Object>> expr {expression()};
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression\n");
        return std::make_unique<Grouping<Object>>(std::move(expr));
    }

    if(match({TokenType::IDENTIFIER})){
        return std::make_unique<Variable<Object>>(previous());
    }

    throw error(peek(), "Expect expression\n");
}

std::unique_ptr<Expr<Object>> Parser::logical_or()
{
    std::unique_ptr<Expr<Object>> expr {logical_and()};
    while(match({TokenType::OR})){
        Token token {previous()};
        std::unique_ptr<Expr<Object>> right {logical_and()};
        expr = std::make_unique<Logical<Object>>(std::move(expr), token, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr<Object>> Parser::logical_and()
{
    std::unique_ptr<Expr<Object>> expr {equality()};
    while(match({TokenType::AND})){
        Token token {previous()};
        std::unique_ptr<Expr<Object>> right {equality()};
        expr = std::make_unique<Logical<Object>>(std::move(expr), token, std::move(right));
    }
    return expr;
}
bool Parser::check(TokenType type)
{
    if(is_at_end()){
        return false;
    }
    return peek().get_type() == type;
}

Token Parser::advance()
{
    if(!is_at_end()){
        current = std::next(current);
    }
    return previous();
}

Token Parser::previous()
{
    return *std::prev(current);
}

bool Parser::is_at_end()
{
    return peek().get_type() == TokenType::END_OF_FILE;
}

Token Parser::peek()
{
    return *current;
}

Token Parser::consume(TokenType type, std::string msg)
{
    if(check(type)){
        return advance();
    }
    throw error(peek(), msg);
}

void Parser::synchronize()
{
    advance();
    while(!is_at_end()){

        if(previous().get_type() == TokenType::SEMICOLON){
            return;
        }

        if(peek().get_type() == TokenType::CLASS
        || peek().get_type() == TokenType::FUN
        || peek().get_type() == TokenType::VAR
        || peek().get_type() == TokenType::FOR
        || peek().get_type() == TokenType::WHILE
        || peek().get_type() == TokenType::IF
        || peek().get_type() == TokenType::PRINT
        || peek().get_type() == TokenType::RETURN){
            return;
        }

        advance();
    }
}

Parser::ParseError Parser::error(Token type, std::string msg)
{
    Lox::error(type, msg);
    return ParseError();
}

bool Parser::match(const std::vector<TokenType> &tokens)
{
    for(TokenType type : tokens){
        if(check(type)){
            advance();
            return true;
        }
    }
    return false;
}

Parser::Parser(std::list<Token> _tokens) :
    tokens{_tokens},
    current {tokens.begin()}
{}

std::vector<std::unique_ptr<Stmt<void>>> Parser::parse()
{

    std::vector<std::unique_ptr<Stmt<void>>> statements;
    while(!is_at_end()){
        statements.push_back(declaration(ParseContext{.is_break_valid=false}));
    }
    return statements;
}
