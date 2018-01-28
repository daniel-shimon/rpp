cmake_minimum_required(VERSION 3.9)
project(rshi)

set(CMAKE_CXX_STANDARD 11)

add_executable(rshi main.cpp Lexer.cpp Lexer.h)