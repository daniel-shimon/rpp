//
// Created by Daniel Shimon on 1/29/2018.
// daielk09@gmail.com
//

#ifndef RSHI_PARSER_H
#define RSHI_PARSER_H

#include "Lexer.h"

class ExpressionVisitor;
class StatementVisitor;
class Value;
class Statement;
class AssignStatement;

// region expressions

class Expression
{
public:
    Value* implicitValue = nullptr;
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
class CallExpression: public Expression
{
public:
    Token* token;
    Expression* callee;
    vector<Expression*> arguments;
    bool index;

    CallExpression(Token* token, Expression* callee, vector<Expression*> arguments, bool index = false) :
            token(token), callee(callee), arguments(arguments), index(index) {};
    Value* accept(ExpressionVisitor* visitor);
};
class FunctionExpression: public Expression
{
public:
    Token* token;
    vector<Token*> arguments;
    Statement* action;

    FunctionExpression(Token* token, vector<Token*> arguments, Statement* action) :
            token(token), arguments(arguments), action(action) {};
    Value* accept(ExpressionVisitor* visitor);
};
class ClassExpression: public Expression
{
public:
    Token* token;
    vector<AssignStatement*> actions;

    ClassExpression(Token* token, vector<AssignStatement*> actions) :
            token(token), actions(actions) {};
    Value* accept(ExpressionVisitor* visitor);
};
class GetExpression: public Expression
{
public:
    Expression* callee;
    Token* token;
    string name;

    GetExpression(Expression* callee, Token* token, string name = "") :
            callee(callee), token(token), name(name) {};
    Value* accept(ExpressionVisitor* visitor);
};

// endregion

// region statements

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
class WhileStatement : public Statement
{
public:
    Expression* condition;
    Statement* action;

    WhileStatement(Expression* condition, Statement* action): condition(condition), action(action) {};
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
class SetStatement : public Statement
{
public:
    Expression* callee;
    Token* name;
    Expression* value;

    SetStatement(Expression* callee, Token* name, Expression* value): callee(callee), name(name), value(value) {};
    void accept(StatementVisitor* visitor);
};
class TryStatement : public Statement
{
public:
    Statement* action;
    vector<Expression*> filters;
    vector<pair<Token*, Statement*>> catches;
    Statement* elseAction;
    Statement* finallyAction;

    TryStatement(Statement *action, vector<Expression *> filters, vector<pair<Token*, Statement*>> catches,
                 Statement *elseAction, Statement *finallyAction) :
            action(action), filters(filters), catches(catches),
            elseAction(elseAction), finallyAction(finallyAction) {};
    void accept(StatementVisitor* visitor);
};
class ForStatement : public Statement
{
public:
    Token* name;
    Expression* iterator;
    Statement* action;

    ForStatement(Token* name, Expression* iterator, Statement* action) :
            name(name), iterator(iterator), action(action) {};
    void accept(StatementVisitor* visitor);
};

// endregion

class ExpressionVisitor
{
public:
    virtual Value* evaluate(Expression* expression) = 0;
    virtual Value* evaluateBinary(BinaryExpression* binary) = 0;
    virtual Value* evaluateUnary(UnaryExpression* unary) = 0;
    virtual Value* evaluateLiteral(LiteralExpression* literal) = 0;
    virtual Value* evaluateGrouping(GroupingExpression* grouping) = 0;
    virtual Value* evaluateVariable(VariableExpression* variable) = 0;
    virtual Value* evaluateCall(CallExpression* variable) = 0;
    virtual Value* evaluateFunction(FunctionExpression* variable) = 0;
    virtual Value* evaluateClass(ClassExpression* variable) = 0;
    virtual Value* evaluateGet(GetExpression* variable) = 0;
};

class StatementVisitor
{
public:
    virtual Value* executeExpression(ExpressionStatement* statement) = 0;
    virtual void executeBlock(BlockStatement* statement) = 0;
    virtual void executeCommand(CommandStatement *statement) = 0;
    virtual void executeIf(IfStatement* statement) = 0;
    virtual void executeWhile(WhileStatement* statement) = 0;
    virtual void executeAssign(AssignStatement* statement) = 0;
    virtual void executeSet(SetStatement* statement) = 0;
    virtual void executeTry(TryStatement* statement) = 0;
    virtual void executeFor(ForStatement* statement) = 0;
};

class Parser
{
private:
    vector<Token*> tokens;
    int current = 0;
    int indent = 0;

    Expression* expression();
    Expression* function();
    Expression* klass();
    Expression* equality();
    Expression* comparison();
    Expression* addition();
    Expression* multiplication();
    Expression* power();
    Expression* unary();
    Expression* call();
    Expression* primary();

    Statement* statement();
    Statement* commandStatement(bool allowValue = true);
    Statement* ifStatement();
    Statement* whileStatement();
    Statement* tryStatement();
    Statement* forStatement();
    Statement* assignStatement();
    Statement* blockStatement(bool enableEmpty);
    Statement* actionStatement(bool enableEmpty = false);
    Statement* defStatement();
    Statement* classStatement();

    Token* next();
    Token* peek();
    bool nextMatch(TokenType type);
    bool isAtEnd();
    bool match(TokenType type, int offset = 0);
    bool peekMatch(initializer_list<TokenType> types);
    bool indented();
    bool nextIndented();
    bool indentedMatch(TokenType type);
    void syntaxError(string message = "unexpected symbol");

    Expression* parseBinary(Expression* (Parser::*parseFunction)(), initializer_list<TokenType> typesList);
public:
    Parser(vector<Token*> tokens): tokens(tokens) {};
    vector<Statement*> parse();
};


#endif //RSHI_PARSER_H
