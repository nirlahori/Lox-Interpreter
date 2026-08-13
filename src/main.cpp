#include <exception>
#include <iostream>

#include "expr.hpp"
#include "astprinter.hpp"
#include "lox.hpp"


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
