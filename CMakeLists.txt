cmake_minimum_required(VERSION 3.9)
project(rshi)

set(CMAKE_CXX_STANDARD 11)

add_executable(rshi src/main.cpp src/Lexer.cpp src/Lexer.h src/Parser.cpp src/Parser.h src/Interpreter.cpp src/Interpreter.h)