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
    Value(bool value): type(Bool), value((void*)value) {};
    Value(double value): type(Number), value(new double(value)) {};
    Value(double* value): type(Number), value(value) {};
    Value(string value): type(String), value(new string(value)) {};
    Value(string* value): type(String), value(value) {};
    Value(): type(NoneType), value(nullptr) {};
    double getNumber();
    bool getBool();
    string getString();
    string toString();
};

class Environment
{
private:
    map<string, Value*> variables;
    Environment* enclosing;

public:
    Environment(Environment* enclosing = nullptr) : enclosing(enclosing) {};
    void set(string name, Value* value);
    Value* get(string name);
    Environment* getEnclosing();
    ~Environment() {
        enclosing = nullptr;
    }
};

class Interpreter : ExpressionVisitor, StatementVisitor {
private:
    static map<uint32_t, string> hebrew;
    Environment* environment;

    bool truthEvaluation(Value* value);
    bool equalityEvaluation(Value *first, Value *second);
public:
    Interpreter() {
        environment = new Environment();
    };
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
    void executeWhile(WhileStatement* statement);
    void executeAssign(AssignStatement* statement);
    void executeBlock(BlockStatement* statement);

    static void runtimeError(Token* token, string message = "unsupported operator");
    static map<uint32_t, string> setupHebrew();
    static void print(Value* value, bool printNone = true);

    static string englishify(Value *value);
};


#endif //RSHI_INTERPRETER_H
