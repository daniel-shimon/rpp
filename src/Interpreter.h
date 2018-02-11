//
// Created by Daniel Shimon on 2/5/2018.
//

#ifndef RSHI_INTERPRETER_H
#define RSHI_INTERPRETER_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "Parser.h"

class FunctionValue;

enum ValueType
{
    String, Number, Bool, NoneType, Function,
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
    Value(FunctionValue* value): type(Function), value(value) {};
    Value(): type(NoneType), value(nullptr) {};
    double getNumber();
    bool getBool();
    string getString();
    FunctionValue* getFunction();
    string toString();
};

struct ReturnValue
{
    Token* token;
    Value* value;
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

class Interpreter : public ExpressionVisitor, public StatementVisitor {
public:
    static map<uint32_t, string> hebrew;
    static vector<pair<string, Value*>> globals;
    Environment* environment;

    Interpreter() {
        environment = new Environment();
        for (pair<string, Value*> variable : globals)
            environment->set(variable.first, variable.second);
    };

    Value* evaluate(Expression* expression);
    Value* evaluateBinary(BinaryExpression* binary);
    Value* evaluateUnary(UnaryExpression* unary);
    Value* evaluateLiteral(LiteralExpression* literal);
    Value* evaluateGrouping(GroupingExpression* grouping);
    Value* evaluateVariable(VariableExpression* variable);
    Value* evaluateCall(CallExpression* call);

    Value* execute(vector<Statement*> statements);
    void executeExpression(ExpressionStatement* statement);
    void executeCommand(CommandStatement *statement);
    void executeIf(IfStatement* statement);
    void executeWhile(WhileStatement* statement);
    void executeAssign(AssignStatement* statement);
    void executeBlock(BlockStatement* statement);

    static bool truthEvaluation(Value* value);
    static bool equalityEvaluation(Value *first, Value *second);
    static void runtimeError(Token* token, string message = "unsupported operator");
    static map<uint32_t, string> setupHebrew();
    static void print(Value* value, bool printNone = true, bool printEndLine = true);
    static string englishify(Value *value);
};

class FunctionValue
{
public:
    int arity;

    virtual Value* call(Interpreter* interpreter, vector<Value*> arguments) = 0;
};

class DeclaredFunction : public FunctionValue
{
private:
    vector<string> argumentNames;
    Statement* action;

public:
    DeclaredFunction(vector<string> argumentNames, Statement* actions) :
            argumentNames(argumentNames), action(actions) {
        arity = argumentNames.size();
    };
    Value* call(Interpreter* interpreter, vector<Value*> arguments);
};

class NativeFunction : public FunctionValue
{
private:
    void* nativeCall;
public:
    NativeFunction(int arity, Value* (nativeCall)(Interpreter*, vector<Value*>)) : nativeCall((void*)nativeCall) {
        this->arity = arity;
    };
    Value* call(Interpreter* interpreter, vector<Value*> arguments);
};

#endif //RSHI_INTERPRETER_H
