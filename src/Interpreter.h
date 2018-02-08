//
// Created by Daniel Shimon on 2/5/2018.
//

#ifndef RSHI_INTERPRETER_H
#define RSHI_INTERPRETER_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "Parser.h"

enum ValueType
{
    String, Number, Bool, NoneType,
};

class Value
{
public:
    void* value;
    ValueType type;
    Token* token = nullptr;
    static Value* None;

    Value(ValueType type, void* value): type(type), value(value) {};
    double getNumber();
    bool getBool();
    string getString();
    string toString();
};

class Environment
{
private:
    map<string, Value*> variables;

public:
    Environment() {};
    void set(string name, Value* value);
    Value* get(string name);
};

class Interpreter : ExpressionVisitor, StatementVisitor {
private:
    static map<uint32_t, string> hebrew;
    Environment environment;

    bool truthEvaluation(Value* value);
    bool equalityEvaluation(Value *first, Value *second);
public:
    Interpreter() {};
    Value* evaluate(Expression* expression);
    Value* evaluateBinary(BinaryExpression* binary);
    Value* evaluateUnary(UnaryExpression* unary);
    Value* evaluateLiteral(LiteralExpression* literal);
    Value* evaluateGrouping(GroupingExpression* grouping);
    Value* evaluateVariable(VariableExpression* variable);

    Value* execute(vector<Statement*> statements);
    void executeExpression(ExpressionStatement* statement);
    void executeCommand(CommandStatement *statement);
    void executeIf(IfStatement* statement);
    void executeAssign(AssignStatement* statement);
    void executeBlock(BlockStatement* statement);

    static void runtimeError(Token* token, string message = "unsupported operator");
    static map<uint32_t, string> setupHebrew();
    static void print(Value* value, bool printNone = true);

    static string englishify(Value *value);
};


#endif //RSHI_INTERPRETER_H
