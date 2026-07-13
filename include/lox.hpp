#ifndef LOX_HPP
#define LOX_HPP

#include <string>
#include <cstring>
#include <string_view>
#include <cstdio>
#include <sys/stat.h>

class Lox
{

    static bool had_error;

public:
    Lox() = default;

    static void report(int line, std::string_view where, std::string_view msg);
    static void error(int line, std::string_view msg);
    void run(std::string source);
    void run_file(std::string_view path);
    void run_prompt();

};

#endif // LOX_HPP
