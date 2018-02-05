//
// Created by Daniel Shimon on 2/5/2018.
//

#ifndef RSHI_INTERPRETER_H
#define RSHI_INTERPRETER_H

#include <math.h>

#include "Parser.h"

enum ValueType
{
    String, Number, Bool, NoneType,
};

class Value
{
public:
    ValueType type;
    void* value;

    Value(ValueType type, void* value): type(type), value(value) {};
    double getNumber();
    bool getBool();
    string getString();
};

class Interpreter : Visitor {
private:
    bool truthEvaluation(Value* value);
public:
    Interpreter() {};
    Value* evaluate(Expression* expression);
    Value* evaluateBinary(Binary* binary);
    Value* evaluateUnary(Unary* unary);
    Value* evaluateLiteral(Literal* literal);
    Value* evaluateGrouping(Grouping* grouping);

    bool equalityEvaluation(Value *first, Value *second);
};


#endif //RSHI_INTERPRETER_H
