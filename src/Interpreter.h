//
// Created by Daniel Shimon on 2/5/2018.
//

#ifndef RSHI_INTERPRETER_H
#define RSHI_INTERPRETER_H

#include <math.h>
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

    Value(ValueType type, void* value): type(type), value(value) {};
    double getNumber();
    bool getBool();
    string getString();
};

class Interpreter : ExpressionVisitor, StatementVisitor {
private:
    static map<uint32_t, string> hebrew;

    bool truthEvaluation(Value* value);
    bool equalityEvaluation(Value *first, Value *second);
    static void print(Value* value);
public:
    Interpreter() {};
    Value* evaluate(Expression* expression);
    Value* evaluateBinary(BinaryExpression* binary);
    Value* evaluateUnary(UnaryExpression* unary);
    Value* evaluateLiteral(LiteralExpression* literal);
    Value* evaluateGrouping(GroupingExpression* grouping);

    void execute(vector<Statement*> statements);
    void executeExpression(ExpressionStatement* statement);
    void executePrint(PrintStatement* statement);

    static void runtimeError(Token* token, string message);
    static map<uint32_t, string> setupHebrew();
};


#endif //RSHI_INTERPRETER_H
