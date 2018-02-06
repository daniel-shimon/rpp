//
// Created by Dan The Man on 1/29/2018.
//

#include "Parser.h"

Expression* Parser::expression()
{
    return equality();
}

Expression* Parser::equality()
{
    return parseBinary(this->comparison, {Equals, NotEquals});
}

Expression* Parser::comparison()
{
    return parseBinary(this->addition, {Bigger, Smaller, BiggerEq, SmallerEq});
}

Expression* Parser::addition()
{
    return parseBinary(this->multiplication, {Plus, Minus});
}

Expression* Parser::multiplication()
{
    return parseBinary(this->power, {Divide, Multiply, Modulo});
}

Expression* Parser::power()
{
    return parseBinary(this->unary, {Power});
}

Expression* Parser::unary()
{
    if (peekMatch({Not, Minus}))
    {
        Token* op = next();
        Expression* expression = unary();
        return new Unary(op, expression);
    }

    return primary();
}

Expression* Parser::primary()
{
    if (peekMatch({False, True, None, StringLiteral, NumberLiteral}))
    {
        return new Literal(next());
    }

    if (nextMatch(RightParen))
    {
        Expression* value = expression();
        if (nextMatch(LeftParen))
            return new Grouping(value);

        syntaxError("missing (");
    }

    syntaxError("unsupported symbol");
}

Token* Parser::next() {
    Token* token = tokens[current];
    current++;
    return token;
}

Token* Parser::peek() {
    return tokens[current];
}

bool Parser::peekMatch(initializer_list<TokenType> typesList) {
    vector<TokenType> types = vector<TokenType>(typesList.begin(), typesList.end());

    for (TokenType type : types)
        if (check(type))
            return true;

    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd())
        return false;

    return (peek()->type == type);
}

bool Parser::isAtEnd() {
    return peek()->type == EndOfFile;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
Expression* Parser::parseBinary(Expression* (Parser::*parseFunction)(), initializer_list<TokenType> typesList) {
    Expression* expression = (this->*parseFunction)();

    while(peekMatch(typesList))
    {
        Token* op = next();
        Expression* second = (this->*parseFunction)();
        expression = new Binary(expression, op, second);
    }

    return expression;
}
#pragma clang diagnostic pop

bool Parser::nextMatch(TokenType type) {
    if (check(type))
    {
        next();
        return true;
    }

    return false;
}

Expression* Parser::parse() {
    return expression();
}

void Parser::syntaxError(string message) {
    throw RPPException("Syntax Error", tokens[current]->errorSignature(), message);
}

Value* Grouping::accept(Visitor *Visitor) {
    return Visitor->evaluateGrouping(this);
}

Value* Literal::accept(Visitor *Visitor) {
    return Visitor->evaluateLiteral(this);
}

Value* Unary::accept(Visitor *Visitor) {
    return Visitor->evaluateUnary(this);
}

Value* Binary::accept(Visitor *Visitor) {
    return Visitor->evaluateBinary(this);
}
