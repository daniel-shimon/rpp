//
// Created by Dan The Man on 1/24/2018.
//

#ifndef RSHI_LEXER_H
#define RSHI_LEXER_H

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "utf8.h"

using namespace std;

enum TokenType
{
    RightParen, LeftParen, Plus, Minus, Divide, Multiply,

    Identifier, String, Number,

    And, Or, Def, Print, If, Else, Elif, Not, Equals, NotEquals,

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


const map<string, TokenType> reserved = {
        {"וגם", TokenType::And},
        {"או", TokenType::Or},
        {"הגדר", TokenType::Def},
        {"כתוב", TokenType::Print},
        {"אלולא", TokenType::If},
        {"אחרת", TokenType::Else},
        {"אואם", TokenType::Elif},
        {"לא", TokenType::Not},
        {"שווהל", TokenType::Equals},
        {"שונהמ", TokenType::NotEquals},
};


class Lexer {
private:
    string *source;
    vector<Token> tokens;
    string::iterator start;
    string::iterator iterator;
    string::iterator end;
    unsigned int index = 1;
    unsigned int line = 1;
    void addToken(TokenType type, void* value = nullptr);

    bool isAtEnd();
    uint32_t next();
    bool nextMatches(char ch);
    uint32_t peek(int offset = 0);

    static bool isDigit(uint32_t ch);
    static bool isAlpha(uint32_t ch);

    void scanString(char delimiter = '"');
    void scanNumber();
    void scanIdentifier();

    string errorSignature();

public:
    explicit Lexer(string* source);
    vector<Token> scanTokens();
};


#endif //RSHI_LEXER_H
