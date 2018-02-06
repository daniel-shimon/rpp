#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

map<uint32_t, string> hebrew;

void print(Value* value)
{
    switch (value->type)
    {
        case String:
        {
            string s = value->getString();
            string::iterator it = s.begin();
            while (it != s.end()) {
                string::iterator prev = it;
                uint32_t ch = utf8::next(it, s.end());
                if (hebrew[ch].empty())
                {
                    cout << string(prev, it);
                }
                else
                    cout << hebrew[ch];
            }
            cout << endl;
            break;
        }
        case Number:
            cout << value->getNumber();
            break;
        case Bool:
        {
            string v = value->getBool() ? "True" : "False";
            cout << v;
            break;
        }
        case NoneType:
            cout << "None";
            break;
    }
}

void setup() {
    string alphabet = "אבגדהוזחטיכלמנסעפצקרשתםףץןך";
    string::iterator it = alphabet.begin();
    hebrew[utf8::next(it, alphabet.end())] = "a";
    hebrew[utf8::next(it, alphabet.end())] = "b";
    hebrew[utf8::next(it, alphabet.end())] = "g";
    hebrew[utf8::next(it, alphabet.end())] = "d";
    hebrew[utf8::next(it, alphabet.end())] = "h";
    hebrew[utf8::next(it, alphabet.end())] = "w";
    hebrew[utf8::next(it, alphabet.end())] = "z";
    hebrew[utf8::next(it, alphabet.end())] = "j";
    hebrew[utf8::next(it, alphabet.end())] = "t";
    hebrew[utf8::next(it, alphabet.end())] = "y";
    hebrew[utf8::next(it, alphabet.end())] = "c";
    hebrew[utf8::next(it, alphabet.end())] = "l";
    hebrew[utf8::next(it, alphabet.end())] = "m";
    hebrew[utf8::next(it, alphabet.end())] = "n";
    hebrew[utf8::next(it, alphabet.end())] = "s";
    hebrew[utf8::next(it, alphabet.end())] = "'a";
    hebrew[utf8::next(it, alphabet.end())] = "p";
    hebrew[utf8::next(it, alphabet.end())] = "tz";
    hebrew[utf8::next(it, alphabet.end())] = "k";
    hebrew[utf8::next(it, alphabet.end())] = "r";
    hebrew[utf8::next(it, alphabet.end())] = "sh";
    hebrew[utf8::next(it, alphabet.end())] = "t";
    hebrew[utf8::next(it, alphabet.end())] = "m";
    hebrew[utf8::next(it, alphabet.end())] = "f";
    hebrew[utf8::next(it, alphabet.end())] = "tz";
    hebrew[utf8::next(it, alphabet.end())] = "n";
    hebrew[utf8::next(it, alphabet.end())] = "j";
}

int main(int argC, char** argV) {
    string* source;
    if (argC <= 1)
        source = new string("12%10");
    else if (argC == 2)
    {
        ifstream file(argV[1]);
        stringstream buffer;
        buffer << file.rdbuf();
        source = new string(buffer.str());
    }
    else if (argC == 3 && string(argV[1]) == "-c")
        source = new string(argV[2]);
    else
    {
        cout << "usage:" << endl << "\trpp path" << endl << "\trpp -c code" << endl;
        return 1;
    }

    setup();

    try
    {
        Lexer *lexer = new Lexer(source);
        Parser *parser = new Parser(lexer->scan());
        Interpreter *interpreter = new Interpreter();

        Value *value = interpreter->evaluate(parser->parse());

        print(value);
    } catch (RPPException exception)
    {
        cout << exception.what() << endl;
        return 1;
    }

    return 0;
}
