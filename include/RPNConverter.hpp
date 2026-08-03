#ifndef RPNCONVERTER_HPP
#define RPNCONVERTER_HPP

#include <string>

#include "visitor.hpp"
#include "expr.hpp"

// ((1 + 2) * (3 - 4))
// 1 2 + 3 4 - *

struct RPNConverter : Visitor<std::string>{
    RPNConverter() = default;

    std::string print(Expr<std::string>* expr){
        return expr->accept(this);
    }

    std::string visit(Binary<std::string>* bin){
        std::string left_str = bin->left->accept(this);
        std::string right_str = bin->right->accept(this);
        std::string opr_str = bin->opr.get_lexeme() + " ";
        return left_str + right_str + opr_str;
    }

    std::string visit(Unary<std::string>* unr){
        return "not implemented";
    }

    std::string visit(Grouping<std::string>* grp){
        std::string str = grp->expression->accept(this);
        return str;
    }

    std::string visit(Literal<std::string>* lit){
        return std::string(lit->value) + " ";
   }
};

#endif // RPNCONVERTER_HPP
