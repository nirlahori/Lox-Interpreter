#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include <cstring>
#include <string_view>
#include <cstdio>
#include <sys/stat.h>

#include "token.hpp"
#include "runtimeerror.hpp"
#include "interpreter.hpp"

class Lox
{
    Interpreter interpreter;
    static bool had_error;
    static bool had_runtime_error;
public:
    Lox() = default;

    static void report(int line, std::string_view where, std::string_view msg);
    static void error(int line, std::string_view msg);
    static void error(Token type, std::string msg);
    static void runtime_error(RuntimeError error);
    void run(std::string source);
    void run_file(std::string_view path);
    void run_prompt();

};

#endif // LOX_HPP
