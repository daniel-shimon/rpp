//
// Created by Daniel Shimon on 1/24/2018.
// daielk09@gmail.com
//

#ifndef RSHI_LEXER_H
#define RSHI_LEXER_H

#include "utf8.h"
#include "Hebrew.h"

#include <memory>
#define TokenType _TokenType // handle winapi override

enum TokenType
{
    OpenParen, CloseParen, OpenSquare, CloseSquare,

    Bigger, Smaller, BiggerEq, SmallerEq, Assign,

    Plus, Minus, Divide, Multiply, Modulo, Power,

    Identifier, StringLiteral, NumberLiteral, False, True, None,

    And, Or, Not, Equals, NotEquals,

    If, Else, While, Def, ClassDef, Try, Catch, As, Finally, For, In,

    Print, Exit, Return, Throw, Break, Continue,

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
    ~Token();
    string errorSignature();
    static string errorSignature(int line, int index, string lexeme = "");
};

//region Reserved

const map<string, TokenType> reserved = {
        {"וגם", And},
        {"או", Or},
        {"פעולה", Def},
        {"הדפס", Print},
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
        {"זרוק", Throw},
        {"נסה", Try},
        {"תפוס", Catch},
        {"בתור", As},
        {"לבסוף", Finally},
        {"לכל", For},
        {"בתוך", In},
        {"שבור", Break},
        {"המשך", Continue},
};

//endregion

class Lexer {
private:
    vector<shared_ptr<Token>> tokens;
    string source;
    string::iterator iterator;
    string::iterator end;
    unsigned int index = 1;
    unsigned int line = 1;
    inline vector<shared_ptr<Token>> _scan();
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
    explicit Lexer(string&& source);
    vector<shared_ptr<Token>> scan();
};


#endif //RSHI_LEXER_H
