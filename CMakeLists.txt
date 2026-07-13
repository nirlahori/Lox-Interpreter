cmake_minimum_required(VERSION 3.16)

project(Lox_Interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Lox_Interpreter main.cpp
    tokentype.hpp
    token.hpp token.cpp
    scanner.hpp scanner.cpp
    lox.hpp lox.cpp
)

include(GNUInstallDirs)
install(TARGETS Lox_Interpreter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
