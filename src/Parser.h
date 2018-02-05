//
// Created by Dan The Man on 1/29/2018.
//

#ifndef RSHI_PARSER_H
#define RSHI_PARSER_H

#include "Lexer.h"

class Expression
{
public:
    class Binary : Expression
    {
    private:
        Expression left;
        Token op;
        Expression right;
    public:
        Binary(Expression left, Token op, Expression right) : left(left), op(op), right(right) {};
    };
};


class Parser {
private:
    vector<Token> tokens;
    int current = 0;

    Expression expression();
    Expression equality();
    Expression comparison();

    Token next();
    Token peek();
    bool isAtEnd();
    bool check(TokenType type);
    bool peekMatch(initializer_list<TokenType> types);

public:
    Parser(vector<Token> tokens): tokens(tokens);
};


#endif //RSHI_PARSER_H
