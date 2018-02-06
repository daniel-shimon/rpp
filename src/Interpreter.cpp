//
// Created by Daniel Shimon on 2/5/2018.
//

#include "Interpreter.h"

Value* Interpreter::evaluate(Expression* expression) {
    return expression->accept(this);
}

Value* Interpreter::evaluateLiteral(Literal* literal) {
    switch (literal->token->type)
    {
        case False:
            return new Value(Bool, (void*)false);
        case True:
            return new Value(Bool, (void*)true);
        case None:
            return new Value(NoneType, nullptr);
        case StringLiteral:
            return new Value(String, literal->token->value);
        case NumberLiteral:
            return new Value(Number, literal->token->value);
    }

    runtimeError(literal->token, "unsupported literal");
}

Value* Interpreter::evaluateGrouping(Grouping *grouping) {
    return this->evaluate(grouping->value);
}

Value* Interpreter::evaluateUnary(Unary *unary) {
    Value* value = evaluate(unary->expression);
    value->token = unary->op;

    switch (unary->op->type)
    {
        case Not:
            return new Value(Bool, (void*)!truthEvaluation(value));
        case Minus:
            return new Value(Number, new double(-value->getNumber()));
    }

    runtimeError(unary->op, "unsupported operator");
}

bool Interpreter::truthEvaluation(Value* value) {
    return value == 0;
}

Value* Interpreter::evaluateBinary(Binary *binary) {
    Value* first = evaluate(binary->first);
    first->token = binary->op;
    Value* second = evaluate(binary->second);
    second->token = binary->op;

    switch (binary->op->type)
    {
        case Equals:
            return new Value(Bool, (void*)equalityEvaluation(first, second));
        case NotEquals:
            return new Value(Bool, (void*)!equalityEvaluation(first, second));
        case Bigger:
            return new Value(Bool, (void*)(first->getNumber() > second->getNumber()));
        case Smaller:
            return new Value(Bool, (void*)(first->getNumber() < second->getNumber()));
        case BiggerEq:
            return new Value(Bool, (void*)(first->getNumber() >= second->getNumber()));
        case SmallerEq:
            return new Value(Bool, (void*)(first->getNumber() <= second->getNumber()));
        case Plus:
            if (first->type == Number && second->type == Number)
                return new Value(Number, new double(first->getNumber() + second->getNumber()));
            if (first->type == String && second->type == String)
                return new Value(String, new string(first->getString() + second->getString()));
            break;
        case Minus:
            return new Value(Number, new double(first->getNumber() - second->getNumber()));
        case Divide:
            return new Value(Number, new double(first->getNumber() / second->getNumber()));
        case Multiply:
            if (first->type == Number && second->type == Number)
                return new Value(Number, new double(first->getNumber() * second->getNumber()));
            if ((first->type == String && second->type == Number) || (first->type == Number && second->type == String))
            {
                string repeat;
                int count;
                if (first->type == String && second->type == Number)
                {
                    repeat = first->getString();
                    count = (int) second->getNumber();
                } else
                {
                    repeat = second->getString();
                    count = (int) first->getNumber();
                }
                string value = "";
                for (; count > 0; count--)
                    value += repeat;
                return new Value(String, new string(value));
            }
        case Power:
            return new Value(Number, new double(pow(first->getNumber(), second->getNumber())));
        case Modulo:
            return new Value(Number, new double((int) first->getNumber() % (int) second->getNumber()));
    }

    runtimeError(binary->op, "unsupported operator");
}

bool Interpreter::equalityEvaluation(Value* first, Value* second) {
    if (first->type == second->type)
        switch (first->type)
        {
            case Bool:
                return first->getBool() == second->getBool();
            case Number:
                return first->getNumber() == second->getNumber();
            case String:
                return first->getString() == second->getString();
        }

    return false;
}

void Interpreter::runtimeError(Token* token, string message) {
    throw RPPException("Runtime Error", token->errorSignature(), message);
}

double Value::getNumber() {
    if (type != Number)
        Interpreter::runtimeError(token, "value is not a Number");
    return *(double*)value;
}

bool Value::getBool() {
    if (type != Bool)
        Interpreter::runtimeError(token, "value is not a Bool");
    return (bool)value;
}

string Value::getString() {
    if (type != String)
        Interpreter::runtimeError(token, "value is not a String");
    return *(string*)value;
}
