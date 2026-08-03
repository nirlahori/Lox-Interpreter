#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP


#include <string>
#include <vector>
#include <sstream>

#include "visitor.hpp"
#include "expr.hpp"


class AstPrinter : Visitor<std::string>
{
    std::string parenthesize(std::string name, const std::vector<Expr<std::string>*>& exprs){

        std::ostringstream str;
        str << "(" << name;
        for(Expr<std::string>* expr : exprs){
            str << " ";
            str << expr->accept(this);
        }
        str << ")";
        return str.str();
    }


public:
    AstPrinter() = default;

    template<typename T>
    std::string print(Expr<T>* expr){
        return expr->accept(this);
    }

    std::string visit(Binary<std::string>* bin){
        std::vector<Expr<std::string>*> vec{bin->left, bin->right};
        return parenthesize(bin->opr.get_lexeme(), vec);
    }

    std::string visit(Unary<std::string>* un){
        std::vector<Expr<std::string>*> vec{un->right};
        return parenthesize(un->opr.get_lexeme(), vec);
    }

    std::string visit(Grouping<std::string>* grp){
        std::vector<Expr<std::string>*> vec{grp->expression};
        return parenthesize("group", vec);
    }

    std::string visit(Literal<std::string>* lit){
        if(lit->value == nullptr){
            return "nil";
        }
        return lit->value;
    }
};

#endif // ASTPRINTER_HPP
