cmake_minimum_required(VERSION 3.9)
project(rpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(rpp src/main.cpp src/Lexer.cpp src/Lexer.h src/Parser.cpp src/Parser.h src/Interpreter.cpp src/Interpreter.h src/Hebrew.cpp src/Hebrew.h src/RPP.cpp src/RPP.h)