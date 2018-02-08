//
// Created by Dan The Man on 1/29/2018.
//

#include "Parser.h"

// region expressions

Expression* Parser::expression()
{
    return parseBinary(equality, {And, Or});
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
        return new LiteralExpression(next());

    if (match(Identifier))
        return new VariableExpression(next());

    if (nextMatch(RightParen))
    {
        Expression* value = expression();
        if (nextMatch(LeftParen))
            return new GroupingExpression(value);

        syntaxError("missing (");
    }

    syntaxError();
}

// endregion

// region statements

Statement *Parser::statement() {
    if (peekMatch({Print, Exit}))
        return commandStatement();
    if (nextMatch(If))
        return ifStatement();
    if (nextMatch(While))
        return whileStatement();

    return assignStatement();
}

Statement *Parser::ifStatement() {
    Expression* condition = expression();
    Statement* action = actionStatement();

    vector<pair<Expression*, Statement*>> elifs;
    while (match(Else) && match(If, 1))
    {
        next();
        next();
        Expression* elifCondition = expression();
        Statement* elifAction = actionStatement();
        pair<Expression*, Statement*> elif(elifCondition, elifAction);
        elifs.push_back(elif);
    }

    Statement* elseAction = nullptr;
    if (nextMatch(Else))
        elseAction = actionStatement();

    return new IfStatement(condition, action, elifs, elseAction);
}

Statement *Parser::whileStatement() {
    Expression* condition = expression();
    Statement* action = actionStatement();

    return new WhileStatement(condition, action);
}

Statement *Parser::assignStatement() {
    Expression* first = expression();

    if (match(Assign))
    {
        if (VariableExpression* variable = dynamic_cast<VariableExpression*>(first))
        {
            next();
            Expression* value = expression();
            return new AssignStatement(variable->token, value);
        }

        syntaxError("invalid assignment target");
    }

    return new ExpressionStatement(first);
}

Statement *Parser::commandStatement() {
    Token* command = next();
    return new CommandStatement(command, expression());
}

Statement *Parser::blockStatement() {
    indent++;
    vector<Statement*> statements = parse();
    indent--;

    if (statements.size() == 0)
        syntaxError("Empty code block");

    return new BlockStatement(statements);
}

Statement *Parser::actionStatement() {
    if (nextMatch(Colon))
    {
        if (!nextMatch(NewLine))
            syntaxError("newline not allowed after colon");

        return blockStatement();
    }
    return statement();
}

// endregion

// region utils

Token* Parser::next() {
    Token* token = tokens[current];
    current++;
    return token;
}

Token* Parser::peek() {
    return tokens[current];
}

bool Parser::isAtEnd() {
    return peek()->type == EndOfFile;
}

bool Parser::peekMatch(initializer_list<TokenType> typesList) {
    vector<TokenType> types = vector<TokenType>(typesList.begin(), typesList.end());

    for (TokenType type : types)
        if (match(type))
            return true;

    return false;
}

bool Parser::match(TokenType type, int offset) {
    if (tokens.size() - current < offset + 1)
        return false;

    return (tokens[current+offset]->type == type);
}

bool Parser::nextMatch(TokenType type) {
    if (match(type))
    {
        next();
        return true;
    }

    return false;
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

vector<Statement*> Parser::parse() {
    vector<Statement*> statements;
    vector<RPPException> exceptions;

    while (!isAtEnd())
    {
        try
        {
            while(nextMatch(NewLine)) {}
            bool out = false;
            for (int i = 0; i < indent; i++)
                if (!match(Indent, i))
                {
                    out = true;
                    break;
                }
            if (out)
                break;
            if (nextMatch(NewLine))
                continue;

            current += indent;
            int startLine = peek()->line;

            statements.push_back(statement());

            if (!(peekMatch({Semicolon, NewLine, EndOfFile}) || peek()->line > startLine))
                syntaxError();
        }
        catch (RPPException exception)
        {
            exceptions.push_back(exception);
            while(!peekMatch({Semicolon, NewLine, EndOfFile}))
                next();
        }

        if (peekMatch({Semicolon, NewLine}))
            next();
    }

    if (exceptions.size() > 0)
        throw exceptions;

    return statements;
}

void Parser::syntaxError(string message) {
    throw RPPException("Syntax Error", tokens[current]->errorSignature(), message);
}

// endregion

// region accepts

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

Value *VariableExpression::accept(ExpressionVisitor *visitor) {
    return visitor->evaluateVariable(this);
}

void ExpressionStatement::accept(StatementVisitor* visitor) {
    visitor->executeExpression(this);
}

void CommandStatement::accept(StatementVisitor *visitor) {
    visitor->executeCommand(this);
}

void AssignStatement::accept(StatementVisitor *visitor) {
    return visitor->executeAssign(this);
}

void IfStatement::accept(StatementVisitor *visitor) {
    visitor->executeIf(this);
}

void BlockStatement::accept(StatementVisitor *visitor) {
    visitor->executeBlock(this);
}

void WhileStatement::accept(StatementVisitor *visitor) {
    visitor->executeWhile(this);
}

// endregion