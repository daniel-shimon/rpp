//
// Created by Daniel Shimon on 2/5/2018.
//

#include "Interpreter.h"

// region evaluation

Value* Interpreter::evaluate(Expression* expression) {
    return expression->accept(this);
}

Value* Interpreter::evaluateLiteral(LiteralExpression* literal) {
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

Value* Interpreter::evaluateGrouping(GroupingExpression *grouping) {
    return this->evaluate(grouping->value);
}

Value* Interpreter::evaluateUnary(UnaryExpression *unary) {
    Value* value = evaluate(unary->expression);
    value->token = unary->op;

    switch (unary->op->type)
    {
        case Not:
            return new Value(Bool, (void*)!truthEvaluation(value));
        case Minus:
            return new Value(Number, new double(-value->getNumber()));
    }

    runtimeError(unary->op);
}

Value* Interpreter::evaluateBinary(BinaryExpression *binary) {
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

    runtimeError(binary->op);
}

Value *Interpreter::evaluateVariable(VariableExpression *variable) {
    Value* value = environment.get(*(string*)variable->token->value);

    if (value == nullptr)
        throw RPPException("Name error", variable->token->errorSignature(),
                           *(string*)variable->token->value + " is not defined");

    return value;
}

bool Interpreter::truthEvaluation(Value* value) {
    return value == 0;
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

// endregion

// region execution

Value* Interpreter::execute(vector<Statement *> statements) {
    Value* returnValue = Value::None;
    for (Statement* statement : statements)
    {
        try
        {
            statement->accept(this);
        }
        catch (Value* value)
        {
            returnValue = value;
        }
    }
    return returnValue;
}

void Interpreter::executeExpression(ExpressionStatement *statement) {
    throw evaluate(statement->expression);
}

void Interpreter::executeCommand(CommandStatement *statement) {
    switch (statement->command->type)
    {
        case Print:
            print(evaluate(statement->expression));
            break;
        case Exit:
            exit((int)evaluate(statement->expression)->getNumber());
    }
}

void Interpreter::executeAssign(AssignStatement *statement) {
    environment.set(*(string*)statement->token->value, evaluate(statement->value));
}

// endregion

// region utils

void Interpreter::print(Value* value, bool printNone) {
    switch (value->type)
    {
        case String:
        {
            cout << englishify(value);
            break;
        }
        case Number:
            cout << value->getNumber();
            break;
        case Bool:
        {
            string v = value->getBool() ? "True" : "False";
            cout << v;
            break;
        }
        case NoneType:
            if (printNone)
                cout << "None";
            else
                return;
        }
        cout << endl;
}

string Interpreter::englishify(Value *value) {
    string output = "";
    string origin = value->getString();

    string::iterator it = origin.begin();
    while (it != origin.end())
    {
        string::iterator prev = it;
        uint32_t ch = utf8::next(it, origin.end());
        if (hebrew.count(ch) == 0)
            output += string(prev, it);
        else
            output += hebrew[ch];
    }
    return output;
}

map<uint32_t, string> Interpreter::setupHebrew() {
    string alphabet = "אבגדהוזחטיכלמנסעפצקרשתםףץןך";
    string::iterator it = alphabet.begin();

    map<uint32_t, string> hebrew;
    hebrew[utf8::next(it, alphabet.end())] = "a";
    hebrew[utf8::next(it, alphabet.end())] = "b";
    hebrew[utf8::next(it, alphabet.end())] = "g";
    hebrew[utf8::next(it, alphabet.end())] = "d";
    hebrew[utf8::next(it, alphabet.end())] = "h";
    hebrew[utf8::next(it, alphabet.end())] = "w";
    hebrew[utf8::next(it, alphabet.end())] = "z";
    hebrew[utf8::next(it, alphabet.end())] = "j";
    hebrew[utf8::next(it, alphabet.end())] = "t";
    hebrew[utf8::next(it, alphabet.end())] = "y";
    hebrew[utf8::next(it, alphabet.end())] = "c";
    hebrew[utf8::next(it, alphabet.end())] = "l";
    hebrew[utf8::next(it, alphabet.end())] = "m";
    hebrew[utf8::next(it, alphabet.end())] = "n";
    hebrew[utf8::next(it, alphabet.end())] = "s";
    hebrew[utf8::next(it, alphabet.end())] = "'a";
    hebrew[utf8::next(it, alphabet.end())] = "p";
    hebrew[utf8::next(it, alphabet.end())] = "tz";
    hebrew[utf8::next(it, alphabet.end())] = "k";
    hebrew[utf8::next(it, alphabet.end())] = "r";
    hebrew[utf8::next(it, alphabet.end())] = "sh";
    hebrew[utf8::next(it, alphabet.end())] = "t";
    hebrew[utf8::next(it, alphabet.end())] = "m";
    hebrew[utf8::next(it, alphabet.end())] = "f";
    hebrew[utf8::next(it, alphabet.end())] = "tz";
    hebrew[utf8::next(it, alphabet.end())] = "n";
    hebrew[utf8::next(it, alphabet.end())] = "j";

    return hebrew;
}

map<uint32_t, string> Interpreter::hebrew = Interpreter::setupHebrew();

void Interpreter::runtimeError(Token* token, string message) {
    throw RPPException("Runtime Error", token->errorSignature(), message);
}

void Environment::set(string name, Value *value) {
    variables[name] = value;
}

Value *Environment::get(string name) {
    return variables[name];
}

// endregion

// region values

Value* Value::None = new Value(NoneType, nullptr);

double Value::getNumber() {
    if (type != Number)
        Interpreter::runtimeError(token, "value " + toString() + " is not a Number");
    return *(double*)value;
}

bool Value::getBool() {
    if (type != Bool)
        Interpreter::runtimeError(token, "value " + toString() + " is not a Bool");
    return (bool)value;
}

string Value::getString() {
    if (type != String)
        Interpreter::runtimeError(token, "value " + toString() + " is not a String");
    return *(string*)value;
}

string Value::toString() {
    switch (type)
    {
        case NoneType:
            return "none";
        case Number:
            return to_string(getNumber());
        case Bool:
            if (getBool())
                return "true";
            return "false";
        case String:
            return "'" + Interpreter::englishify(this) + "'";
    }
}

// endregion
