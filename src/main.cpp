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
    } catch (vector<RPPException> exceptions)
    {
        for (RPPException exception : exceptions)
            print(exception.what());

        if (!eval)
            return 1;
    } catch (RPPException exception)
    {
        print(exception.what());

        if (!eval)
            return 1;
    } catch (utf8::invalid_utf8 exception)
    {
        Hebrew::print(exception.what(), true);

        if (!eval)
            return 1;
    }

    return 0;
}

int _main(int argC, char** argV)
{
    string* source = new string();
    int returnValue;
    RPP::init();

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
            print(">");
            getline(cin, *source);
            returnValue = execute(source, interpreter);
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
            string::iterator invalid = utf8::find_invalid(line.begin(), line.end());
            if (invalid != line.end())
            {
                print("invalid UTF-8 at line " + to_string(lineCount));
                return 2;
            }
            lineCount++;
            buff += line + "\n";
        }
        source = new string(buff);

        return execute(source);
    }
    if (argC == 3 && string(argV[1]) == "-c")
    {
        source = new string(argV[2]);
        return execute(source);
    }

    print("usage:\n\trpp [path] [-v] [--version] [-c code] [-i] [--interactive]");
    return 1;
}

int main(int argC, char** argV) {
    int ret = _main(argC, argV);
    #ifdef ComplexOutput
    IO->restore();
    #endif
}