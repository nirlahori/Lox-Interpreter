#include "runtimeerror.hpp"

RuntimeError::RuntimeError(Token _token, std::string _message) :
    std::exception{},
    token {_token},
    message {_message}
    {}

const char *RuntimeError::what() const noexcept
{
    return message.c_str();
}

Token RuntimeError::get_token() const
{
    return token;
}



