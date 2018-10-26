cmake_minimum_required(VERSION 3.5)
project(rpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(rpp src/main.cpp src/Lexer.cpp src/Lexer.h src/Parser.cpp src/Parser.h src/Interpreter.cpp src/Interpreter.h src/Hebrew.cpp src/Hebrew.h src/BuiltIns.cpp src/BuiltIns.h src/Exception.h src/IO.h src/IO.cpp)