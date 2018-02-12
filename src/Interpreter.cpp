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
            return new Value(false);
        case True:
            return new Value(true);
        case None:
            return Value::None;
        case StringLiteral:
            return new Value((string*)literal->token->value);
        case NumberLiteral:
            return new Value((double*)literal->token->value);
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
            return new Value(!truthEvaluation(value));
        case Minus:
            return new Value(-value->getNumber());
    }

    runtimeError(unary->op);
}

Value* Interpreter::evaluateBinary(BinaryExpression *binary) {
    if (binary->op->type == And)
        return new Value(truthEvaluation(evaluate(binary->first)) && truthEvaluation(evaluate(binary->second)));
    if (binary->op->type == Or)
        return new Value(truthEvaluation(evaluate(binary->first)) || truthEvaluation(evaluate(binary->second)));

    Value* first = evaluate(binary->first);
    first->token = binary->op;
    Value* second = evaluate(binary->second);
    second->token = binary->op;

    switch (binary->op->type)
    {
        case Equals:
            return new Value(equalityEvaluation(first, second));
        case NotEquals:
            return new Value(!equalityEvaluation(first, second));
        case Bigger:
            return new Value((first->getNumber() > second->getNumber()));
        case Smaller:
            return new Value((first->getNumber() < second->getNumber()));
        case BiggerEq:
            return new Value((first->getNumber() >= second->getNumber()));
        case SmallerEq:
            return new Value((first->getNumber() <= second->getNumber()));
        case Plus:
            if (first->type == Number && second->type == Number)
                return new Value(first->getNumber() + second->getNumber());
            if (first->type == String && second->type == String)
                return new Value(first->getString() + second->getString());
            break;
        case Minus:
            return new Value(first->getNumber() - second->getNumber());
        case Divide:
            return new Value(first->getNumber() / second->getNumber());
        case Multiply:
            if (first->type == Number && second->type == Number)
                return new Value(first->getNumber() * second->getNumber());
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
                return new Value(value);
            }
        case Power:
            return new Value(pow(first->getNumber(), second->getNumber()));
        case Modulo:
            return new Value((double((int) first->getNumber() % (int) second->getNumber())));
    }

    runtimeError(binary->op);
}

Value *Interpreter::evaluateVariable(VariableExpression *variable) {
    Value* value = environment->get(*(string*)variable->token->value);

    if (value == nullptr)
        throw RPPException("Name error", variable->token->errorSignature(),
                           *(string*)variable->token->value + " is not defined");

    return value;
}

Value *Interpreter::evaluateCall(CallExpression* call) {
    Value* callee = evaluate(call->callee);
    callee->token = call->token;
    FunctionValue* function = callee->getFunction();

    if (function->arity == call->arguments.size() || function->arity == -1)
    {
        vector<Value*> arguments;
        for (Expression* expression : call->arguments)
            arguments.push_back(evaluate(expression));

        return function->call(this, arguments);
    }

    runtimeError(call->token, "invalid argument count to " + callee->toString());
}

Value *Interpreter::evaluateFunction(FunctionExpression *call) {
    vector<string> arguments;

    for (Token* token : call->arguments)
        arguments.push_back(*(string*)token->value);

    return new Value(new DeclaredFunction(arguments, call->action));
}

bool Interpreter::truthEvaluation(Value* value) {
    if (value->type == Bool)
        return value->getBool();
    return value->type != NoneType;
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
        catch (ReturnValue value)
        {
            runtimeError(value.token, "return statement from non-function");
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
            return print(evaluate(statement->expression));
        case Exit:
            return exit((int)evaluate(statement->expression)->getNumber());
        case Return:
            throw ReturnValue(statement->command, evaluate(statement->expression));
    }

    runtimeError(statement->command, "unknown command");
}

void Interpreter::executeAssign(AssignStatement *statement) {
    environment->set(*(string*)statement->token->value, evaluate(statement->value));
}

void Interpreter::executeIf(IfStatement *statement) {
    if (truthEvaluation(evaluate(statement->condition)))
        return statement->action->accept(this);

    for (pair<Expression*, Statement*> elif : statement->elifs)
        if (truthEvaluation(evaluate(elif.first)))
            return elif.second->accept(this);

    if (statement->elseAction != nullptr)
        statement->elseAction->accept(this);
}

void Interpreter::executeBlock(BlockStatement *statement) {
    Environment* newEnvironment = new Environment(environment);
    environment = newEnvironment;

    for (Statement* inlineStatement : statement->statements)
        inlineStatement->accept(this);

    environment = environment->getEnclosing();
    delete newEnvironment;
}

void Interpreter::executeWhile(WhileStatement *statement) {
    while (truthEvaluation(evaluate(statement->condition)))
        statement->action->accept(this);
}

Value *DeclaredFunction::call(Interpreter *interpreter, vector<Value*> arguments) {
    Environment* newEnvironment = new Environment(interpreter->environment, true);
    interpreter->environment = newEnvironment;

    for (int i = 0; i < argumentNames.size(); i++)
        newEnvironment->set(argumentNames[i], arguments[i]);

    Value* value = Value::None;
    try
    {
        action->accept(interpreter);
    }
    catch (ReturnValue returnValue)
    {
        value = returnValue.value;
    }

    interpreter->environment = newEnvironment->getEnclosing();
    delete newEnvironment;

    return value;
}

Value *NativeFunction::call(Interpreter *interpreter, vector<Value *> arguments) {
    Value* value = ((Value* (*)(Interpreter*, vector<Value*>))nativeCall)(interpreter, arguments);

    if (value == nullptr)
        return Value::None;

    return value;
}

// endregion

// region utils

void Interpreter::print(Value* value, bool printNone, bool printEndLine) {
    switch (value->type)
    {
        case String:
        {
            cout << englishify(value);
            break;
        }
        case NoneType:
            if (printNone)
                cout << value->toString();
            else
                return;
        default:
            cout << value->toString();
        }

    if (printEndLine)
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
    if (strict || enclosing == nullptr || enclosing->get(name) == nullptr)
        variables[name] = value;
    else
        enclosing->set(name, value);
}

Value *Environment::get(string name) {
    Value* value = variables[name];
    if (value == nullptr && enclosing != nullptr)
        return enclosing->get(name);
    return value;
}

Environment *Environment::getEnclosing() {
    return enclosing;
}

// endregion

// region values

Value* Value::None = new Value();

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

FunctionValue* Value::getFunction() {
    if (type != Function)
        Interpreter::runtimeError(token, "value " + toString() + " is not a Function");
    return (FunctionValue*)value;
}

string Value::toString() {
    switch (type)
    {
        case NoneType:
            return "none";
        case Number: {
            double value = getNumber();
            if ((int)value == value)
                return to_string((int)value);
            return to_string(getNumber());
        }
        case Bool:
            if (getBool())
                return "true";
            return "false";
        case String:
            return "'" + Interpreter::englishify(this) + "'";
        case Function: {
            int arity = getFunction()->arity;

            if (arity == -1)
                return "function(...)";
            if (arity == 1)
                return "function(" + to_string(arity) + " argument)";
            return "function(" + to_string(arity) + " arguments)";
        }
    }
}

// endregion

// region globals

vector<pair<string, Value*>> Interpreter::globals = {
        {"קלוט", new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
            interpreter->print(arguments[0], false, false);
            string input;
            getline(cin, input);
            return new Value(input);
        }))},
        {"טקסט", new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
            return new Value(arguments[0]->toString());
        }))},
};

// endregion