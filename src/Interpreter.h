//
// Created by Daniel Shimon on 2/5/2018.
// daielk09@gmail.com
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef RSHI_INTERPRETER_H
#define RSHI_INTERPRETER_H

#include <cmath>
#include <cstdlib>

#include "Parser.h"

class Interpreter;
class FunctionValue;
class ClassValue;
class InstanceValue;

// region values

struct ReturnValue
{
public:
    Token* token;
    Value* value;

    ReturnValue(Token* token, Value* value): token(token), value(value) {};
};
struct ContinueValue
{
public:
    Token* token;

    ContinueValue(Token* token): token(token) {};
};
struct BreakValue
{
public:
    Token* token;

    BreakValue(Token* token): token(token) {};
};

enum ValueType
{
    Number, Bool, NoneType, Function, Class, Instance,
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
    Value(int value): type(Number), value(new double(value)) {};
    Value(double value): type(Number), value(new double(value)) {};
    Value(double* value): type(Number), value(value) {};
    Value(FunctionValue* value): type(Function), value(value) {};
    Value(ClassValue* value): type(Class), value(value) {};
    Value(InstanceValue* value): type(Instance), value(value) {};
    Value(): type(NoneType), value(nullptr) {};
    double getNumber();
    bool isString();
    bool getBool();
    string getString();
    FunctionValue* getFunction();
    ClassValue* getClass();
    InstanceValue* getInstance();
    string toString(Interpreter* interpreter = nullptr);
};
class FunctionValue
{
public:
    int arity;
    string name;

    virtual Value* call(Interpreter *interpreter, vector<Value*> arguments = vector<Value*>()) = 0;
};
class ClassValue
{
public:
    string name;
    int initArity;
    map<string, Value*> staticAttributes, methods;

    ClassValue(string name, int initArity, map<string, Value*> staticAttributes, map<string, Value*> methods) :
            staticAttributes(staticAttributes), methods(methods), initArity(initArity), name(name) {};
};
class InstanceValue
{
public:
    ClassValue* klass;
    map<string, Value*> attributes;
    map<string, void*> nativeAttributes;

    InstanceValue(ClassValue* klass) : klass(klass) {};
};
class Environment
{
private:
    map<string, Value*> variables;
    Environment* enclosing;
    bool strict;

public:
    Environment(Environment* enclosing = nullptr, bool strict = false) : enclosing(enclosing), strict(strict) {};
    void set(string name, Value* value);
    Value* get(string name);
    Environment* getEnclosing();
    ~Environment() {
        enclosing = nullptr;
    }
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
class BoundFunction : public FunctionValue
{
private:
    Value* self;
    FunctionValue* function;
public:
    BoundFunction(Value* self, FunctionValue* function, string name) : self(self), function(function) {
        this->arity = function->arity;
        this->name = name;
    };
    Value* call(Interpreter* interpreter, vector<Value*> arguments);
};

// endregion

class Interpreter : public ExpressionVisitor, public StatementVisitor {
public:
    static map<string, Value*> globals;
    Environment* environment;
    Token* currentToken;

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
    Value* evaluateFunction(FunctionExpression* function);
    Value* evaluateClass(ClassExpression* call);
    Value* evaluateGet(GetExpression* call);

    template<bool evaluate> Value* execute(vector<Statement*> statements);
    Value* executeExpression(ExpressionStatement* statement);
    void executeCommand(CommandStatement *statement);
    void executeIf(IfStatement* statement);
    void executeWhile(WhileStatement* statement);
    void executeAssign(AssignStatement* statement);
    void executeBlock(BlockStatement* statement);
    void executeSet(SetStatement* statement);
    void executeTry(TryStatement* statement);
    void executeFor(ForStatement* statement);

    Value *createInstance(Value *callee, Token *token, const vector<Value *> &arguments);
    inline Value *createInstance(Value *callee, const vector<Value *> &arguments)
        {return createInstance(callee, currentToken, arguments);};
    Value *createString(Token *token, string *name);
    inline Value *createString(string name)
        {return createString(currentToken, new string(name));};
    void runtimeError(string message = "unsupported operator");
    void nameError(Token* token, string name);
    void attributeError(Token* token, string callee, string name);
    void print(Value* value, bool printNone = true, bool printEndLine = true);
    bool isInstance(Value* obj, Value* cls);
    static bool truthEvaluation(Value* value);
    static bool equalityEvaluation(Value *first, Value *second);
    static void runtimeError(Token* token, string message = "unsupported operator");

};

#endif //RSHI_INTERPRETER_H

#pragma clang diagnostic pop