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
        return new UnaryExpression(op, expression);
    }

    return primary();
}

Expression* Parser::primary()
{
    if (peekMatch({False, True, None, StringLiteral, NumberLiteral}))
    {
        return new LiteralExpression(next());
    }

    if (nextMatch(RightParen))
    {
        Expression* value = expression();
        if (nextMatch(LeftParen))
            return new GroupingExpression(value);

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
        expression = new BinaryExpression(expression, op, second);
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

vector<Statement*> Parser::parse() {
    vector<Statement*> statements;

    while (!isAtEnd())
    {
        while(nextMatch(NewLine)) {}
        statements.push_back(statement());
        if (peekMatch({Semicolon, NewLine}))
            next();
        else if (isAtEnd())
            break;
        else
            syntaxError("Unexpected symbol");
    }

    return statements;
}

void Parser::syntaxError(string message) {
    throw RPPException("Syntax Error", tokens[current]->errorSignature(), message);
}

Statement *Parser::statement() {
    if (nextMatch(Print))
        return printStatement();

    return expressionStatement();
}

ExpressionStatement *Parser::expressionStatement()
{
    return new ExpressionStatement(expression());
}

Statement *Parser::printStatement() {
    return new PrintStatement(expression());
}

Value* GroupingExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateGrouping(this);
}

Value* LiteralExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateLiteral(this);
}

Value* UnaryExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateUnary(this);
}

Value* BinaryExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateBinary(this);
}

void ExpressionStatement::accept(StatementVisitor* visitor) {
    visitor->executeExpression(this);
}

void PrintStatement::accept(StatementVisitor *visitor) {
    visitor->executePrint(this);
}
