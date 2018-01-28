//
// Created by Dan The Man on 1/24/2018.
//

#ifndef RSHI_PARSER_H
#define RSHI_PARSER_H

#include <string>
#include <vector>
#include <stdexcept>

#include "utf8.h"

using namespace std;

enum TokenType
{
    RightParen, LeftParen, Plus, Minus, Divide, Multiply,

    Identifier, String, Number,

    NewLine, EndOfFile,
};

struct Token
{
    string lexeme;
    TokenType type;
    void* value;
    unsigned int line;
    unsigned int index;

    Token(TokenType type, string lexeme, void* value, unsigned int line, unsigned int index) :
            lexeme(std::move(lexeme)), type(type), value(value), line(line), index(index) {}
};

class Lexer {
private:
    string source;
    vector<Token> tokens;
    string::iterator iterator;
    string::iterator end;
    unsigned int index = 1;
    unsigned int line = 1;
    void addToken(TokenType type, void* value = nullptr);
    bool isAtEnd();
    uint32_t next();
    bool nextMatches(char ch);
    uint32_t peek(int offset = 0);
    void scanString(char delimiter = '"');
    void scanNumber();
    string errorSignature();
public:

    explicit Lexer(string source);
    vector<Token> scanTokens();
    static bool isDigit(uint32_t ch);
};


#endif //RSHI_PARSER_H
