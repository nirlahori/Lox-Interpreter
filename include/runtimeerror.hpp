#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include <exception>
#include <string>
#include "token.hpp"

class RuntimeError : std::exception
{

    Token token;
    std::string message;

public:
    RuntimeError();
    RuntimeError(Token _token, std::string _message);
    const char* what() const noexcept;
    Token get_token() const;

};

#endif // RUNTIMEERROR_HPP
