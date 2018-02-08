//
// Created by Dan The Man on 1/29/2018.
//

#ifndef RSHI_PARSER_H
#define RSHI_PARSER_H

#include "Lexer.h"

class ExpressionVisitor;
class StatementVisitor;
class Value;

class Expression
{
public:
    virtual Value* accept(ExpressionVisitor* visitor) = 0;
};

class BinaryExpression: public Expression
{
public:
    Expression* first;
    Token* op;
    Expression* second;

    BinaryExpression(Expression* first, Token* op, Expression* second) : first(first), op(op), second(second) {};
    Value* accept(ExpressionVisitor* visitor);
};
class UnaryExpression: public Expression
{
public:
    Token* op;
    Expression* expression;

    UnaryExpression(Token* op, Expression* expression) : op(op), expression(expression) {};
    Value* accept(ExpressionVisitor* visitor);
};
class LiteralExpression: public Expression
{
public:
    Token* token;

    LiteralExpression(Token* token) : token(token) {};
    Value* accept(ExpressionVisitor* visitor);
};
class GroupingExpression: public Expression
{
public:
    Expression* value;

    GroupingExpression(Expression* value) : value(value) {};
    Value* accept(ExpressionVisitor* visitor);
};
class VariableExpression: public Expression
{
public:
    Token* token;

    VariableExpression(Token* token) : token(token) {};
    Value* accept(ExpressionVisitor* visitor);
};

class Statement
{
public:
    virtual void accept(StatementVisitor* visitor) = 0;
};

class ExpressionStatement : public Statement
{
public:
    Expression* expression;

    ExpressionStatement(Expression* expression): expression(expression) {};
    void accept(StatementVisitor* visitor);
};
class BlockStatement : public Statement
{
public:
    vector<Statement*> statements;

    BlockStatement(vector<Statement*> statements): statements(statements) {};
    void accept(StatementVisitor* visitor);
};
class CommandStatement : public Statement
{
public:
    Token* command;
    Expression* expression;

    CommandStatement(Token* command, Expression* expression): command(command), expression(expression) {};
    void accept(StatementVisitor* visitor);
};
class IfStatement : public Statement
{
public:
    Expression* condition;
    Statement* action;
    vector<pair<Expression*, Statement*>> elifs;
    Statement* elseAction;

    IfStatement(Expression* condition, Statement* action, vector<pair<Expression*, Statement*>> elifs,
                Statement* elseAction): condition(condition), action(action), elifs(elifs), elseAction(elseAction) {};
    void accept(StatementVisitor* visitor);
};
class AssignStatement : public Statement
{
public:
    Token* token;
    Expression* value;

    AssignStatement(Token* identifier, Expression* value): token(identifier), value(value) {};
    void accept(StatementVisitor* visitor);
};

class ExpressionVisitor
{
public:
    virtual Value* evaluate(Expression* expression) = 0;
    virtual Value* evaluateBinary(BinaryExpression* binary) = 0;
    virtual Value* evaluateUnary(UnaryExpression* unary) = 0;
    virtual Value* evaluateLiteral(LiteralExpression* literal) = 0;
    virtual Value* evaluateGrouping(GroupingExpression* grouping) = 0;
    virtual Value* evaluateVariable(VariableExpression* variable) = 0;
};

class StatementVisitor
{
public:
    virtual void executeExpression(ExpressionStatement* statement) = 0;
    virtual void executeBlock(BlockStatement* statement) = 0;
    virtual void executeCommand(CommandStatement *statement) = 0;
    virtual void executeIf(IfStatement* statement) = 0;
    virtual void executeAssign(AssignStatement* statement) = 0;
};

class Parser
{
private:
    vector<Token*> tokens;
    int current = 0;
    int indent = 0;

    Expression* expression();
    Expression* equality();
    Expression* comparison();
    Expression* addition();
    Expression* multiplication();
    Expression* power();
    Expression* unary();
    Expression* primary();

    Statement* statement();
    Statement* commandStatement();
    Statement* ifStatement();
    Statement* assignStatement();
    Statement* blockStatement();
    Statement* actionStatement();

    Token* next();
    Token* peek();
    bool nextMatch(TokenType type);
    bool isAtEnd();
    bool match(TokenType type, int offset = 0);
    bool peekMatch(initializer_list<TokenType> types);
    void syntaxError(string message = "unexpected symbol");

    Expression* parseBinary(Expression* (Parser::*parseFunction)(), initializer_list<TokenType> typesList);
public:
    Parser(vector<Token*> tokens): tokens(tokens) {};
    vector<Statement*> parse();
};


#endif //RSHI_PARSER_H
