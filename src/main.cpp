#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

#include <fstream>
#include <sstream>

using namespace std;

string version = "0.1";

int execute(string* source, Interpreter *interpreter = new Interpreter())
{
    try
    {
        Lexer *lexer = new Lexer(source);
        Parser *parser = new Parser(lexer->scan());
        interpreter->execute(parser->parse());

        delete lexer;
        delete parser;
    } catch (RPPException exception)
    {
        cout << exception.what() << endl;
        return 1;
    }

    return 0;
}

int main(int argC, char** argV) {
    string* source = new string();
    int returnValue;
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

    if (argC == 2 && (string(argV[1]) == "-v" || string(argV[1]) == "--version"))
    {
        cout << "rpp version " << version;
        return 0;
    }

    if (argC == 2)
    {
        ifstream file(argV[1]);
        stringstream buffer;
        buffer << file.rdbuf();
        source = new string(buffer.str());
        return execute(source);
    }

    if (argC == 3 && string(argV[1]) == "-c")
    {
        source = new string(argV[2]);
        return execute(source);
    }

    cout << "usage:" << endl << "\trpp path" << endl << "\trpp -c code" << endl;
    return 1;
}
