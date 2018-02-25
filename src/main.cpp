#include "RPP.h"

#include <fstream>

using namespace std;

string version = "0.1";

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
            cout << exception.what() << endl;

        if (!eval)
            return 1;
    } catch (RPPException exception)
    {
        cout << exception.what() << endl ;

        if (!eval)
            return 1;
    }

    return 0;
}

int main(int argC, char** argV)
{
    string* source = new string();
    int returnValue;
    RPP::init();

    if (argC == 2 && (string(argV[1]) == "-v" || string(argV[1]) == "--version"))
    {
        cout << "rpp version " << version;
        return 0;
    }
    if (argC <= 1)
    {
        cout << "Welcome to interactive rpp (" << version << ")!" << endl;
        Interpreter* interpreter = new Interpreter();
        while (true)
        {
            cout << ">";
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
            cout << "could not open '" << argV[1] << "'" << endl;
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
                cout << "invalid UTF-8 at line " << lineCount << endl;
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

    cout << "usage:" << endl << "\trpp [path] [-v] [--version] [-c code] [-i] [--interactive]" << endl;
    return 1;
}
