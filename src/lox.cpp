#include "lox.hpp"
#include "scanner.hpp"

#include <exception>
#include <list>
#include <iostream>

bool Lox::had_error = false;

void Lox::report(int line, std::string_view where, std::string_view msg){
    std::printf("[line \" %d \" Error %s \": \" %s", line, where.data(), msg.data());
    had_error = true;
}

void Lox::error(int line, std::string_view msg){
    report(line, "", msg);
}


void Lox::run(std::string source){

    Scanner scn(source);
    std::list<Token> tokens {scn.scan_tokens()};
    for(const Token& tok : tokens){
        tok.print();
    }
}


void Lox::run_file(std::string_view path){

    std::FILE* prog {std::fopen(path.data(), "r")};
    if(!prog){
        std::terminate();
    }

    struct stat exec_file_buf;
    if(stat(path.data(), &exec_file_buf) < 0){
        std::terminate();
    }

    long file_size {exec_file_buf.st_size};

    std::string input(file_size, '0');
    std::fread(input.data(), file_size, 1, prog);
    std::fclose(prog);
    run(input);

    if(had_error){
        std::terminate();
    }
}

void Lox::run_prompt(){

    //char input[200];
    std::string input(200, '\0');
    for(;;){
        std::printf("> ");
        //std::scanf("%[a-zA-Z0-9]", input);
        std::getline(std::cin, input);
        if(input.empty()){
            break;
        }
        run(input);
        had_error = false;
    }
}
