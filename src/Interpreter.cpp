//
// Created by Daniel Shimon on 2/5/2018.
//

#include "Interpreter.h"

// region evaluation

Value* Interpreter::evaluate(Expression* expression) {
    if (expression->implicitValue)
        return expression->implicitValue;
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
                           Hebrew::englishify(*(string*)variable->token->value) + " is not defined");

    return value;
}

Value *Interpreter::evaluateCall(CallExpression* call) {
    Value* callee = evaluate(call->callee);
    Token *token = call->token;
    callee->token = token;
    currentToken = token;
    vector<Value*> arguments;

    for (Expression* expression : call->arguments)
        arguments.push_back(evaluate(expression));

    if (callee->type == Function) {
        FunctionValue* function = callee->getFunction();

        if (function->arity == arguments.size() || function->arity == -1)
            return function->call(this, arguments);

        runtimeError(token, "invalid argument count to " + callee->toString());
    }

    if (callee->type == Class) {
        Value *instance = createInstance(callee, token, arguments);
        return instance;
    }

    runtimeError(token, callee->toString() + " is not a function nor class");
}

Value *Interpreter::evaluateFunction(FunctionExpression *function) {
    vector<string> arguments;

    for (Token* token : function->arguments)
        arguments.push_back(*(string*)token->value);

    return new Value(new DeclaredFunction(arguments, function->action));
}

Value *Interpreter::evaluateClass(ClassExpression *klass) {
    int initArity = 0;
    map<string, Value*> staticAttributes, methods;
    for (AssignStatement* statement : klass->actions) {
        Value* value = evaluate(statement->value);
        string name = *(string *) statement->token->value;
        if (value->type == Function) {
            methods[name] = value;
            value->getFunction()->name = name;
            if (name == Init)
                initArity = value->getFunction()->arity;
        } else
            staticAttributes[name] = value;
    }
    return new Value(new ClassValue(staticAttributes, methods, initArity));
}

Value *Interpreter::evaluateGet(GetExpression *get) {
    Value* callee = evaluate(get->callee);
    string name = get->name.empty() ? *(string*)get->token->value : get->name;

    Value* value = nullptr;
    if (callee->type == Instance) {
        if (callee->getInstance()->klass->staticAttributes.count(name))
            value = callee->getInstance()->klass->staticAttributes[name];
        else
            value = callee->getInstance()->attributes[name];
    } else if (callee->type == Class) {
        if (callee->getClass()->staticAttributes.count(name))
            value = callee->getClass()->staticAttributes[name];
        else
            value = callee->getClass()->methods[name];
    } else
        runtimeError(get->token, callee->toString() + " is not an instance nor a class");


    if (value == nullptr)
        throw RPPException("Attribute error", get->token->errorSignature(),
                           callee->toString() + " has no attribute " + Hebrew::englishify(name));

    return value;
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

Value* Interpreter::execute(vector<Statement *> statements, bool evaluate) {
    Value* returnValue = Value::None;
    for (Statement* statement : statements)
    {
        try
        {
            if (evaluate)
                if (ExpressionStatement* expression = dynamic_cast<ExpressionStatement*>(statement)) {
                    returnValue = executeExpression(expression);
                    break;
                }
            statement->accept(this);
        }
        catch (ReturnValue value)
        {
            runtimeError(value.token, "return statement from non-function");
        }
        catch (Value* value)
        {
            runtimeError(value->token, value->toString() + " thrown without catch");
        }
    }
    return returnValue;
}

Value* Interpreter::executeExpression(ExpressionStatement *statement) {
    return evaluate(statement->expression);
}

void Interpreter::executeCommand(CommandStatement *statement) {
    Value *value = evaluate(statement->expression);
    value->token = statement->command;
    switch (statement->command->type)
    {
        case Print:
            return print(value);
        case Exit:
            return exit((int) value->getNumber());
        case Return:
            throw ReturnValue(statement->command, value);
        case Throw:
            throw value;
    }

    runtimeError(statement->command, "unknown command");
}

void Interpreter::executeAssign(AssignStatement *statement) {
    string name = *(string*)statement->token->value;
    Value *value = evaluate(statement->value);
    if (value->type == Function) {
        value->getFunction()->name = name;
    } else if (value->type == Class) {
        value->getClass()->name = name;
    }
    environment->set(name, value);
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

    environment = newEnvironment->getEnclosing();
    delete newEnvironment;
}

void Interpreter::executeWhile(WhileStatement *statement) {
    while (truthEvaluation(evaluate(statement->condition)))
        statement->action->accept(this);
}

void Interpreter::executeSet(SetStatement *statement) {
    Value* callee = evaluate(statement->callee);
    Value* value = evaluate(statement->value);
    string name = *(string*)statement->name->value;
    if (callee->getInstance()->klass->staticAttributes.count(name))
        callee->getInstance()->klass->staticAttributes[name] = value;
    else
        callee->getInstance()->attributes[name] = value;
}

void Interpreter::executeTry(TryStatement *statement) {
    bool caught = false;
    Value* thrown = nullptr;
    try {
        statement->action->accept(this);
    } catch (Value* value) {
        for (int i = 0; i < statement->filters.size(); i++)
            if (isInstance(value, evaluate(statement->filters[i])))
            {
                Environment* newEnvironment = new Environment(environment);
                environment = newEnvironment;
                environment->set(*(string*)statement->catches[i].first->value, value);

                statement->catches[i].second->accept(this);

                environment = newEnvironment->getEnclosing();
                delete newEnvironment;

                caught = true;
                break;
            }

        if (!caught)
            thrown = value;
    }

    if (statement->elseAction && !caught && !thrown)
        statement->elseAction->accept(this);

    if (statement->finallyAction)
        statement->finallyAction->accept(this);

    if (thrown)
        throw thrown;
}

void Interpreter::executeFor(ForStatement *statement) {
    GetExpression *getIterExpression = new GetExpression(statement->iterator, statement->name, Iterator);
    CallExpression* callIterExpression = new CallExpression(statement->name, getIterExpression, vector<Expression*>());

    Value* iterator = evaluateCall(callIterExpression);
    callIterExpression->implicitValue = iterator;
    GetExpression *getExpression = new GetExpression(callIterExpression, statement->name, NextItem);
    CallExpression* callExpression = new CallExpression(statement->name, getExpression, vector<Expression*>());
    string name = *(string*)statement->name->value;

    Environment* newEnvironment = new Environment(environment);
    environment = newEnvironment;

    try
    {
        while (true)
        {
            Value* value = evaluateCall(callExpression);
            environment->set(name, value);
            statement->action->accept(this);
        }
    }
    catch (Value* value)
    {
        if (!isInstance(value, globals[StopException]))
            throw value;
    }

    environment = newEnvironment->getEnclosing();
    delete newEnvironment;
    delete getExpression;
    delete callExpression;
    delete getIterExpression;
    delete callIterExpression;
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

Value *BoundFunction::call(Interpreter *interpreter, vector<Value *> arguments) {
    Environment *newEnvironment = new Environment(interpreter->environment);
    interpreter->environment = newEnvironment;
    newEnvironment->set(Self, self);

    Value* value = function->call(interpreter, arguments);

    interpreter->environment = newEnvironment->getEnclosing();
    delete newEnvironment;

    return value;
}

// endregion

// region utils

void Interpreter::print(Value* value, bool printNone, bool printEndLine) {
    switch (value->type)
    {
        case NoneType:
            if (printNone) {
                cout << value->toString();
                break;
            }
            return;
        default:
            cout << value->toString(this);
        }

    if (printEndLine)
        cout << endl;
}

void Interpreter::runtimeError(Token* token, string message) {
    if (token)
        throw RPPException("Runtime Error", token->errorSignature(), message);
    throw RPPException("Runtime Error", "", message);
}

void Interpreter::runtimeError(string message) {
    runtimeError(currentToken, message);
}

void Environment::set(string name, Value *value) {
    if (strict || enclosing == nullptr || enclosing->get(name) == nullptr)
        variables[name] = value;
    else
        enclosing->set(name, value);
}

bool Interpreter::isInstance(Value *obj, Value *cls) {
    return obj->getInstance()->klass == cls->getClass();
}

Value *Interpreter::createInstance(Value *callee, Token *token, const vector<Value *> &arguments) {
    Value* instance = new Value(new InstanceValue(callee->getClass()));
    map<string, Value*> methods;
    for (pair<string, Value*> method : callee->getClass()->methods)
        methods[method.first] = new Value(
                new BoundFunction(instance, method.second->getFunction(), method.first));
    instance->getInstance()->attributes.insert(methods.begin(), methods.end());

    if (Value* init = methods[Init]) {
        int initArity = instance->getInstance()->klass->initArity;
        if (initArity == arguments.size() || initArity == -1)
            init->getFunction()->call(this, arguments);
        else
            runtimeError(token, "invalid argument count to " +
                                callee->toString() + " constructor " + init->toString());
    }
    return instance;
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

ClassValue *Value::getClass() {
    if (type != Class)
        Interpreter::runtimeError(token, "value " + toString() + " is not a Class");
    return (ClassValue*)value;
}

InstanceValue *Value::getInstance() {
    if (type != Instance)
        Interpreter::runtimeError(token, "value " + toString() + " is not an Instance");
    return (InstanceValue*)value;
}

string Value::toString(Interpreter* interpreter) {
    switch (type)
    {
        case NoneType:
            return "none";
        case Number: {
            double value = getNumber();
            string number;

            if ((int)value == value)
                number = to_string((int)value);
            else
                number = to_string(getNumber());

            if (interpreter)
                return number;
            return "<number " + number + ">";
        }
        case Bool:
            if (getBool())
                return "true";
            return "false";
        case String: {
            const string str = Hebrew::englishify(getString());
            if (interpreter)
                return str;
            return "<string '" + str + "'>";
        }
        case Function: {
            int arity = getFunction()->arity;
            string str = "<function";
            if (!getFunction()->name.empty())
                str += " '" + Hebrew::englishify(getFunction()->name) + "'";
            str += ">(";

            if (arity == -1)
                return str + "...)";
            if (arity == 1)
                return str + "1 argument)";

            return str + to_string(arity) + " arguments)";
        }
        case Class: {
            string str = "<class";
            if (!getClass()->name.empty())
                str += " '" + Hebrew::englishify(getClass()->name) + "'";
            str += '>';

            return str;
        }
        case Instance: {
            if (interpreter && getInstance()->attributes.count(ToString))
                return getInstance()->attributes[ToString]->getFunction()->call(interpreter)->toString(interpreter);

            string str = "<";
            if (!getInstance()->klass->name.empty())
                str +=  "'" + Hebrew::englishify(getInstance()->klass->name) + "' ";
            str += "instance>";

            return str;
        }
    }
}

// endregion

// region globals

map<string, Value*> Interpreter::globals = {
        {"קלוט", new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
            interpreter->print(arguments[0], false, false);
            string input;
            getline(cin, input);
            return new Value(input);
        }))},
        {"טקסט", new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
            return new Value(arguments[0]->toString(interpreter));
        }))},
        {"מספר", new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
            if (arguments[0]->type == Number)
                return new Value(arguments[0]->getNumber());
            return new Value(stod(arguments[0]->getString()));
        }))},
        {"סוג", new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
            return new Value(arguments[0]->toString());
        }))},
        {StopException, new Value(new ClassValue(map<string, Value*>(), map<string, Value*>(), 0, StopException))},
};

// endregion
