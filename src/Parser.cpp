//
// Created by Daniel Shimon on 1/29/2018.
// daielk09@gmail.com
//

#include "Parser.h"

// region expressions

Expression* Parser::expression() {
    if (nextMatch(Def))
        return function();
    if (nextMatch(ClassDef))
        return klass();
    return parseBinary(equality, {And, Or});
}

Expression *Parser::function() {
    if (match(OpenParen))
    {
        Token* token = next();
        vector<Token*> arguments;

        if (match(Identifier))
            do
                arguments.push_back(next());
            while (nextMatch(Comma) && match(Identifier));

        if (!nextMatch(CloseParen))
            syntaxError("missing '(' at end of argument list");

        Statement* action = actionStatement();
        return new FunctionExpression(token, arguments, action);
    }

    syntaxError("missing argument list after function declaration");
}

Expression *Parser::klass() {
    if (nextMatch(Colon) && match(NewLine)) {
        Token* token = next();
        vector<AssignStatement*> actions;
        indent++;
        for (Statement* statement : parse()) {
            if (AssignStatement* assign = dynamic_cast<AssignStatement*>(statement))
                actions.push_back(assign);
            else
                syntaxError("non assign statement in class definition");
        }
        indent--;
        return new ClassExpression(token, actions);
    }

    syntaxError("bad class declaration");
}

Expression* Parser::equality() {
    return parseBinary(this->comparison, {Equals, NotEquals});
}

Expression* Parser::comparison() {
    return parseBinary(this->addition, {Bigger, Smaller, BiggerEq, SmallerEq});
}

Expression* Parser::addition() {
    return parseBinary(this->multiplication, {Plus, Minus});
}

Expression* Parser::multiplication() {
    return parseBinary(this->power, {Divide, Multiply, Modulo});
}

Expression* Parser::power() {
    return parseBinary(this->unary, {Power});
}

Expression* Parser::unary() {
    if (peekMatch({Not, Minus}))
    {
        Token* op = next();
        Expression* expression = unary();
        return new UnaryExpression(op, expression);
    }

    return call();
}

Expression *Parser::call() {
    Expression* callee = primary();

    while (true)
    {
        if (match(OpenParen) || match(OpenSquare))
        {
            Token* token = next();
            vector<Expression*> arguments;

            if (!match(CloseParen) && !match(CloseSquare))
                do {
                    arguments.push_back(expression());
                }
                while (nextMatch(Comma));

            switch (token->type)
            {
                case OpenParen:
                {
                    if (!nextMatch(CloseParen))
                        syntaxError("missing '(' at end of argument list");

                    callee = new CallExpression(token, callee, arguments);
                    break;
                }
                case OpenSquare:
                {
                    if (!nextMatch(CloseSquare))
                        syntaxError("missing '[' at end of argument list");

                    callee = new CallExpression(token, new GetExpression(callee, token, GetItem), arguments, true);
                }
            }
        } else if (nextMatch(Dot))
        {
            if (!match(Identifier))
                syntaxError("missing identifier after '.'");

            callee = new GetExpression(callee, next());
        } else
            break;
    }

    return callee;
}

Expression* Parser::primary() {
    if (peekMatch({False, True, None, StringLiteral, NumberLiteral}))
        return new LiteralExpression(next());

    if (match(Identifier))
        return new VariableExpression(next());

    if (nextMatch(OpenParen))
    {
        Expression* value = expression();
        if (nextMatch(CloseParen))
            return new GroupingExpression(value);

        syntaxError("missing '('");
    }

    syntaxError();
}

// endregion

// region statements

Statement *Parser::statement() {
    if (peekMatch({Print, Exit, Return, Throw}))
        return commandStatement();
    if (nextMatch(If))
        return ifStatement();
    if (nextMatch(While))
        return whileStatement();
    if (nextMatch(Try))
        return tryStatement();
    if (nextMatch(For))
        return forStatement();
    if (match(Identifier, 1) && nextMatch(Def))
        return defStatement();
    if (match(Identifier, 1) && nextMatch(ClassDef))
        return classStatement();

    return assignStatement();
}

Statement *Parser::ifStatement() {
    Expression* condition = expression();
    Statement* action = actionStatement();
    vector<pair<Expression*, Statement*>> elifs;
    Statement* elseAction = nullptr;

    while (indentedMatch(Else))
    {
        if (nextMatch(If)) {
            Expression* elifCondition = expression();
            Statement* elifAction = actionStatement();
            pair<Expression*, Statement*> elif(elifCondition, elifAction);
            elifs.push_back(elif);
        } else {
            elseAction = actionStatement();
            break;
        }
    }

    return new IfStatement(condition, action, elifs, elseAction);
}

Statement *Parser::whileStatement() {
    Expression* condition = expression();
    Statement* action = actionStatement();
    return new WhileStatement(condition, action);
}

Statement *Parser::tryStatement() {
    Statement* action = actionStatement();
    vector<Expression*> filters;
    vector<pair<Token*, Statement*>> catches;
    while (indentedMatch(Catch))
    {
        filters.push_back(expression());

        if (nextMatch(As) && match(Identifier))
        {
            Token* identifier = next();
            Statement* catchAction = actionStatement();
            catches.push_back(pair<Token*, Statement*>(identifier, catchAction));
        } else
            syntaxError("bad catch block");
    }

    if (!filters.size())
        syntaxError("missing 'catch' after 'try' statement");

    Statement* elseAction = nullptr;
    if (indentedMatch(Else))
        elseAction = actionStatement();

    Statement* finallyAction = nullptr;
    if (indentedMatch(Finally))
        finallyAction = actionStatement();

    return new TryStatement(action, filters, catches, elseAction, finallyAction);
}

Statement *Parser::assignStatement() {
    Expression* first = expression();

    if (nextMatch(Assign))
    {
        if (VariableExpression* variable = dynamic_cast<VariableExpression*>(first))
        {
            Expression* value = expression();
            return new AssignStatement(variable->token, value);
        } else if (GetExpression* get = dynamic_cast<GetExpression*>(first))
        {
            Expression* value = expression();
            return new SetStatement(get->callee, get->token, value);
        } else if (CallExpression* call = dynamic_cast<CallExpression*>(first))
            if (GetExpression* getExpr = dynamic_cast<GetExpression*>(call->callee))
                if (getExpr->name == GetItem)
                {
                    Expression* value = expression();
                    call->arguments.push_back(value);
                    getExpr->name = SetItem;
                    return new ExpressionStatement(call);
                }

        syntaxError("invalid assignment target");
    }

    return new ExpressionStatement(first);
}

Statement *Parser::commandStatement() {
    Token* command = next();
    return new CommandStatement(command, expression());
}

BlockStatement *Parser::blockStatement(bool enableEmpty) {
    indent++;
    vector<Statement*> statements = parse();
    indent--;

    if (!enableEmpty && statements.size() == 0)
        syntaxError("Empty code block");

    return new BlockStatement(statements);
}

Statement *Parser::actionStatement() {
    Statement* action;
    if (nextMatch(Colon))
    {
        if (!nextMatch(NewLine))
            syntaxError("newline missing after colon");

        action = blockStatement();
    } else
        action = statement();

    while (nextMatch(NewLine)) {}
    return action;
}

Statement *Parser::defStatement() {
    Token* name = next();
    Expression* value = function();

    return new AssignStatement(name, value);
}

Statement *Parser::classStatement() {
    Token* name = next();
    Expression* value = klass();

    return new AssignStatement(name, value);
}

Statement *Parser::forStatement() {
    if (match(Identifier))
    {
        Token* name = next();

        if (nextMatch(In))
        {
            Expression* iterator = expression();
            Statement* action = actionStatement();
            return new ForStatement(name, iterator, action);
        }

        syntaxError("missing 'in' after 'for' identifier");
    }

    syntaxError("missing identifier after 'for' statement");
}

// endregion

// region utils

Token* Parser::next() {
    Token* token = tokens[current];
    current++;
    return token;
}

Token* Parser::peek() {
    return tokens[current];
}

bool Parser::isAtEnd() {
    return peek()->type == EndOfFile;
}

bool Parser::peekMatch(initializer_list<TokenType> typesList) {
    vector<TokenType> types = vector<TokenType>(typesList.begin(), typesList.end());

    for (TokenType type : types)
        if (match(type))
            return true;

    return false;
}

bool Parser::match(TokenType type, int offset) {
    if (tokens.size() - current < offset + 1)
        return false;

    return (tokens[current+offset]->type == type);
}

bool Parser::nextMatch(TokenType type) {
    if (match(type))
    {
        current++;
        return true;
    }

    return false;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
Expression* Parser::parseBinary(Expression* (Parser::*parseFunction)(), initializer_list<TokenType> typesList) {
    Expression* expression = (this->*parseFunction)();

    while(peekMatch(typesList))
    {
        Token* op = next();
        Expression* second = (this->*parseFunction)();
        expression = new BinaryExpression(expression, op, second);
    }

    return expression;
}
#pragma clang diagnostic pop

vector<Statement*> Parser::parse() {
    vector<Statement*> statements;
    vector<RPPException> exceptions;

    while (!isAtEnd())
    {
        try
        {
            while(nextMatch(NewLine)) {}
            if (!nextIndented())
                break;
            if (nextMatch(NewLine))
                continue;

            int startLine = peek()->line;
            statements.push_back(statement());

            if (!(peekMatch({Semicolon, NewLine, EndOfFile}) || peek()->line > startLine))
                syntaxError();
        }
        catch (RPPException exception)
        {
            exceptions.push_back(exception);
            while(!peekMatch({Semicolon, NewLine, EndOfFile}))
                next();
        }

        if (peekMatch({Semicolon, NewLine}))
            next();
    }

    if (exceptions.size() > 0)
        throw exceptions;

    return statements;
}

bool Parser::indented() {
    for (int i = 0; i < indent; i++)
        if (!match(Indent, i))
            return false;
    return true;
}

bool Parser::nextIndented() {
    if (indented())
    {
        current += indent;
        return true;
    }
    return false;
}

bool Parser::indentedMatch(TokenType type) {
    if (tokens[current-1]->line < peek()->line)
        if (indented() && match(type, indent)) {
            current += indent + 1;
            return true;
        } else
            return false;
    else return nextMatch(type);
}

void Parser::syntaxError(string message) {
    throw RPPException("Syntax Error", tokens[current]->errorSignature(), message);
}

// endregion

// region accepts

Value* GroupingExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateGrouping(this);
}

Value* LiteralExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateLiteral(this);
}

Value* UnaryExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateUnary(this);
}

Value* BinaryExpression::accept(ExpressionVisitor* visitor) {
    return visitor->evaluateBinary(this);
}

Value *VariableExpression::accept(ExpressionVisitor *visitor) {
    return visitor->evaluateVariable(this);
}

Value *CallExpression::accept(ExpressionVisitor *visitor) {
    return visitor->evaluateCall(this);
}

void ExpressionStatement::accept(StatementVisitor* visitor) {
    visitor->executeExpression(this);
}

void CommandStatement::accept(StatementVisitor *visitor) {
    visitor->executeCommand(this);
}

void AssignStatement::accept(StatementVisitor *visitor) {
    return visitor->executeAssign(this);
}

void IfStatement::accept(StatementVisitor *visitor) {
    visitor->executeIf(this);
}

void BlockStatement::accept(StatementVisitor *visitor) {
    visitor->executeBlock(this);
}

void WhileStatement::accept(StatementVisitor *visitor) {
    visitor->executeWhile(this);
}

void SetStatement::accept(StatementVisitor *visitor) {
    visitor->executeSet(this);
}

Value *FunctionExpression::accept(ExpressionVisitor *visitor) {
    return visitor->evaluateFunction(this);
}

Value *ClassExpression::accept(ExpressionVisitor *visitor) {
    return visitor->evaluateClass(this);
}

Value *GetExpression::accept(ExpressionVisitor *visitor) {
    return visitor->evaluateGet(this);
}

void TryStatement::accept(StatementVisitor *visitor) {
    visitor->executeTry(this);
}

void ForStatement::accept(StatementVisitor *visitor) {
    visitor->executeFor(this);
}

// endregion
