//
// Created by Daniel Shimon on 1/28/2018.
// daielk09@gmail.com
//

#include "Interpreter.h"
#include "BuiltIns.h"

#include <cstring>
#include <fstream>

#define version "0.2"

void print(const string& s) {
    Hebrew::print(s, true, false);
}

template<bool eval>
int _execute(string&& source, Interpreter& interpreter)
{
    try
    {
        Lexer lexer(move(source));
        Parser parser(lexer.scan());

        if constexpr (eval) {
            Value* value = interpreter.execute<true>(parser.parse());
            interpreter.print(value, false);
        }
        else
            interpreter.execute<false>(parser.parse());

    } catch (vector<RPPException>& exceptions)
    {
        for (const RPPException& exception : exceptions)
            print(exception.what());

        if constexpr (!eval)
            return 1;
    } catch (RPPException& exception)
    {
        print(exception.what());

        if constexpr (!eval)
            return 1;
    }

    return 0;
}

inline int execute(string&& source) {
    Interpreter interpreter{};
    return _execute<false>(move(source), interpreter);
}

inline int execute(string&& source, Interpreter& interpreter) {
    return _execute<true>(move(source), interpreter);
}

int _main(int argC, char** _argV)
{
    int returnValue;
    RPP::init();

    #ifdef ComplexOutput
    IOInit();
    auto argV = IO->cmdArguments(argC, _argV);
    #else
    #define argV _argV
    #endif

    if (argC == 2 && (argV[1] == "-v" || argV[1] == "--version"))
    {
        print("rpp version " version);
        return 0;
    }
    if (argC == 2 && (argV[1] == "-i" || argV[1] == "--interactive"))
    {
        print("Welcome to interactive rpp (" version ")!");
        Interpreter interpreter{};
        while (true)
        {
            Hebrew::print("> ", false);
            returnValue = execute(Hebrew::read(), interpreter);
            if (returnValue != 0)
                return returnValue;
        }
    }
    if (argC == 2 && argV[1][0] != '-')
    {
        ifstream file(argV[1]);
        if (!file.is_open())
        {
            print("could not open '" + string(argV[1]) + "'");
            return 2;
        }

        string buff;
        string line;
        unsigned int lineCount = 1;
        while (getline(file, line))
        {
            lineCount++;
            buff += line + "\n";
        }

        return execute(move(buff));
    }
    if (argC == 3 && argV[1] == "-c")
    {
        return execute(string(argV[2]));
    }

    print("usage:\n\r\trpp [path] [-v] [--version] [-c code] [-i] [--interactive]");
    return 1;
}

int main(int argC, char** argV) {
    try {
        int ret = _main(argC, argV);
        safeExit(ret);
    } catch (const exception& e) {
        warning(e.what())
    }
    try {
        safeExit(-1);
    } catch (const exception& e) {
        warning(e.what())
    }
    return -2;
}