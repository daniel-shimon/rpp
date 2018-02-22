//
// Created by Dan The Man on 1/24/2018.
//

#ifndef RSHI_LEXER_H
#define RSHI_LEXER_H

#include "Hebrew.h"

#define Self "אני"
#define Init "__התחל__"
#define ToString "__טקסט__"

enum TokenType
{
    OpenParen, CloseParen, Plus, Minus, Divide, Multiply, Modulo,

    Identifier, StringLiteral, NumberLiteral, False, True, None,

    And, Or, Def, If, Else, Not, Equals, NotEquals, Power, While, ClassDef,

    Bigger, Smaller, BiggerEq, SmallerEq, Assign,

    Print, Exit, Return,

    Semicolon, NewLine, EndOfFile, Indent, Colon, Comma, Dot,
};

class Token
{
public:
    string lexeme;
    TokenType type;
    void* value;
    unsigned int line;
    unsigned int index;

    Token(TokenType type, string lexeme, void* value, unsigned int line, unsigned int index) :
            lexeme(std::move(lexeme)), type(type), value(value), line(line), index(index) {}
    string errorSignature();
    static string errorSignature(int line, int index, string lexeme = "");
};

class RPPException : public exception
{
private:
    string type;
    string signature;
    string message;
public:
    RPPException(string type, string signature, string message = "") :
            type(type), signature(signature), message(message) {};
    virtual const char* what() const throw();
};

//region Reserved

const map<string, TokenType> reserved = {
        {"וגם", And},
        {"או", Or},
        {"פעולה", Def},
        {"פלוט", Print},
        {"צא", Exit},
        {"החזר", Return},
        {"אם", If},
        {"אלולא", If},
        {"אחרת", Else},
        {"כלעוד", While},
        {"לא", Not},
        {"שווהל", Equals},
        {"שונהמ", NotEquals},
        {"גדולמ", Bigger},
        {"קטןמ", Smaller},
        {"חלקי", Divide},
        {"כפול", Multiply},
        {"שקר", False},
        {"אמת", True},
        {"ריק", None},
        {"מחלקה", ClassDef},
};

//endregion

class Lexer {
private:
    vector<Token*> tokens;
    string::iterator start;
    string::iterator iterator;
    string::iterator end;
    unsigned int index = 1;
    unsigned int line = 1;
    void addToken(TokenType type, string lexeme, void* value = nullptr);

    bool isAtEnd();
    uint32_t next();
    bool nextMatches(char ch);
    uint32_t peek(int offset = 0);

    static bool isDigit(uint32_t ch);
    static bool isAlpha(uint32_t ch);

    void scanString(char delimiter = '"');
    void scanNumber();
    void scanIdentifier();

public:
    explicit Lexer(string* source);
    vector<Token*> scan();
};


#endif //RSHI_LEXER_H
