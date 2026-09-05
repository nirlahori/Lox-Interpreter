cmake_minimum_required(VERSION 3.16)

project(Lox_Interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Werror -fsanitize=address")

add_executable(Lox_Interpreter src/main.cpp
    include/tokentype.hpp
    include/token.hpp src/token.cpp
    include/scanner.hpp src/scanner.cpp
    include/lox.hpp src/lox.cpp
    include/visitor.hpp  include/object.hpp include/expr.hpp
    include/parser.hpp
    src/parser.cpp
    include/interpreter.hpp
    src/interpreter.cpp
    include/runtimeerror.hpp src/runtimeerror.cpp
    include/stmt.hpp
    include/environment.hpp src/environment.cpp

)


include_directories(BEFORE "include/")


include(GNUInstallDirs)
install(TARGETS Lox_Interpreter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
