#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP


#include <string>
#include <vector>
#include <sstream>
#include <iterator>

#include "visitor.hpp"
#include "expr.hpp"


class AstPrinter : Visitor<Object>
{

    Object parenthesize(std::string name, const std::vector<std::unique_ptr<Expr<Object>>>& exprs){

        std::ostringstream str;
        str << "(" << name;

        std::vector<std::unique_ptr<Expr<Object>>>::const_iterator first = exprs.cbegin();
        while(first != exprs.cend()){
            str << " ";
            str << (*first)->accept(this);
            first = std::next(first);
        }

        str << ")";
        return str.str();
    }


public:
    AstPrinter() = default;

    template<typename T>
    Object print(Expr<T>* expr){
        return expr->accept(this);
    }

    Object visit(Binary<Object>* bin){
        std::vector<std::unique_ptr<Expr<Object>>> vec;
        vec.push_back(std::move(bin->left));
        vec.push_back(std::move(bin->right));
        return parenthesize(bin->opr.get_lexeme(), vec);
    }

    Object visit(Unary<Object>* un){
        std::vector<std::unique_ptr<Expr<Object>>> vec;
        vec.push_back(std::move(un->right));
        return parenthesize(un->opr.get_lexeme(), vec);
    }

    Object visit(Grouping<Object>* grp){
        std::vector<std::unique_ptr<Expr<Object>>> vec;
        vec.push_back(std::move(grp->expression));
        return parenthesize("group", vec);
    }

    Object visit(Literal<Object>* lit){
        if(!lit->value){
            return nullptr;
        }
        return lit->value;
    }
};

#endif // ASTPRINTER_HPP
