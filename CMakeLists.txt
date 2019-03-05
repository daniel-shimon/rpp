cmake_minimum_required(VERSION 3.5)
project(rpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(rpp src/main.cpp
        src/Lexer.cpp src/Lexer.h
        src/Parser.cpp src/Parser.h
        src/Interpreter.cpp src/Interpreter.h
        src/Hebrew.cpp src/Hebrew.h
        src/BuiltIns.cpp src/BuiltIns.h
        src/IO.h src/IO.cpp src/utf8.h
        src/io/generic.h src/io/win.h
        src/io/Unix.cpp src/io/Unix.h
        src/Exception.h)
