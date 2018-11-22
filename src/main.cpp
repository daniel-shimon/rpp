//
// Created by Daniel Shimon on 1/28/2018.
// daielk09@gmail.com
//

#include "Interpreter.h"
#include "BuiltIns.h"

#include <fstream>

string version = "0.1";

void print(string s) {
    Hebrew::print(s, true, false);
}

int execute(string* source, Interpreter *interpreter = nullptr)
{
    bool eval = false;
    if (interpreter)
        eval = true;
    else
        interpreter = new Interpreter();

    try
    {
        Lexer *lexer = new Lexer(source);
        vector<Token *> tokens = lexer->scan();
        Parser *parser = new Parser(tokens);

        if (eval) {
            Value* value = interpreter->execute(parser->parse(), true);
            interpreter->print(value, false);
        } else
            interpreter->execute(parser->parse());

        delete lexer;
        delete parser;
    } catch (vector<RPPException>& exceptions)
    {
        for (RPPException exception : exceptions)
            print(exception.what());

        if (!eval)
            return 1;
    } catch (RPPException& exception)
    {
        print(exception.what());

        if (!eval)
            return 1;
    }

    return 0;
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

    if (argC == 2 && (string(argV[1]) == "-v" || string(argV[1]) == "--version"))
    {
        print("rpp version " + version);
        return 0;
    }
    if (argC == 2 && (string(argV[1]) == "-i" || string(argV[1]) == "--interactive"))
    {
        print("Welcome to interactive rpp (" + version + ")!");
        Interpreter* interpreter = new Interpreter();
        while (true)
        {
            Hebrew::print(">", false);
            auto source = Hebrew::read();
            returnValue = execute(&source, interpreter);
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

        string buff = "";
        string line;
        unsigned int lineCount = 1;
        while (getline(file, line))
        {
            lineCount++;
            buff += line + "\n";
        }

        return execute(new string(buff));
    }
    if (argC == 3 && string(argV[1]) == "-c")
    {
        return execute(new string(argV[2]));
    }

    print("usage:\n\r\trpp [path] [-v] [--version] [-c code] [-i] [--interactive]");
    return 1;
}

int main(int argC, char** argV) {
    try {
        int ret = _main(argC, argV);
        safeExit(ret);
    } catch (exception& e) {
        warning(e.what())
    }
    try {
        safeExit(-1);
    } catch (exception& e) {
        warning(e.what())
    }
    return -2;
}