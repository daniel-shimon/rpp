#include "Lexer.h"
#include "Node.h"

#include <iostream>
#include <map>

using namespace std;

map<uint32_t, string> hebrew;

void print(string s)
{
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

int main() {
    setup();
    string source = "'שלום'";
    Lexer* lexer = new Lexer(source);
    Node* root = new Node();
    Node* currentNode = root;

    for (Token token : lexer->scanTokens())
    {
        if (token.type == TokenType::String)
        {;
            print(*(string*)token.value);
        } else if (token.type == TokenType::Number)
        {
            cout << *(double*)token.value << endl;
        }

        cout << token.type << endl;
    }
//
//    source = "1234";
//    string::iterator it = source.begin();
//    while (it < source.end())
//    {
//        string g = string(source.begin(), it);
//        utf8::advance(it, 0, source.end());
//        uint32_t c = utf8::peek_next(it, source.end());
//        utf8::next(it, source.end());
//        cout << (char)c << endl;
//    }
}
