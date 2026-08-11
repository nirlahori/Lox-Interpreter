#include <exception>
#include <iostream>

#include "expr.hpp"
#include "astprinter.hpp"
#include "lox.hpp"
#include "RPNConverter.hpp"

/*
void print_rpn(){

    // RPN (Reverse-Polish Notation converter Visitor)

    Literal<std::string> l1(1);
    Literal<std::string> l2(2);
    Literal<std::string> l3(4);
    Literal<std::string> l4(3);


    Binary<std::string> left_opr(&l1, Token(TokenType::PLUS, "+", nullptr, 1), &l2);
    Binary<std::string> right_opr(&l3, Token(TokenType::PLUS, "-", nullptr, 1), &l4);

    Grouping<std::string> left_grp (&left_opr);
    Grouping<std::string> right_grp (&right_opr);

    Binary<std::string> bin_expr (&left_grp, Token(TokenType::STAR, "*", nullptr, 1), &right_grp);

    //Grouping<std::string> grp_expr(&bin_expr);
    //Expr<std::string>* expr (&grp_expr);

    std::cout << RPNConverter().print(&bin_expr) << std::endl;
}
*/

int main(int argc, char *argv[])
{
    if (argc > 2) {
        std::fprintf(stdin, "Usage: lox [script]");
        std::terminate();
    }

    Lox l;
    if (argc == 2) {
        l.run_file(argv[1]);
    } else {
        l.run_prompt();
    }
    return 0;
}
