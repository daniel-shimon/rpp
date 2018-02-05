//
// Created by Dan The Man on 1/29/2018.
//

#ifndef RSHI_PARSER_H
#define RSHI_PARSER_H

#include "Lexer.h"

class Visitor;
class Value;

class Expression
{
public:
    virtual Value* accept(Visitor* Visitor) {};
};

class Binary: public Expression
{
public:
    Expression* first;
    Token op;
    Expression* second;

    Binary(Expression* first, Token op, Expression* second) : first(first), op(op), second(second) {};
    Value* accept(Visitor* Visitor);
};
class Unary: public Expression
{
public:
    Token op;
    Expression* expression;

    Unary(Token op, Expression* expression) : op(op), expression(expression) {};
    Value* accept(Visitor* Visitor);
};
class Literal: public Expression
{
public:
    Token token;

    Literal(Token token) : token(token) {};
    Value* accept(Visitor* Visitor);
};
class Grouping: public Expression
{
public:
    Expression* value;

    Grouping(Expression* value) : value(value) {};
    Value* accept(Visitor* Visitor);
};

class Visitor {
public:
    virtual Value* evaluate(Expression* expression) = 0;
    virtual Value* evaluateBinary(Binary* binary) = 0;
    virtual Value* evaluateUnary(Unary* unary) = 0;
    virtual Value* evaluateLiteral(Literal* literal) = 0;
    virtual Value* evaluateGrouping(Grouping* grouping) = 0;
};


class Parser {
private:
    vector<Token> tokens;
    int current = 0;

    Expression* expression();
    Expression* equality();
    Expression* comparison();
    Expression* addition();
    Expression* multiplication();
    Expression* power();
    Expression* unary();
    Expression* primary();

    Token next();
    Token peek();
    bool nextMatch(TokenType type);
    bool isAtEnd();
    bool check(TokenType type);
    bool peekMatch(initializer_list<TokenType> types);

    Expression* parseBinary(Expression* (Parser::*parseFunction)(), initializer_list<TokenType> typesList);
public:
    Parser(vector<Token> tokens): tokens(tokens) {};
    Expression* parse();
};


#endif //RSHI_PARSER_H
