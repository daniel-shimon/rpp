//
// Created by Dan The Man on 1/29/2018.
//

#include "Parser.h"

Expression Parser::expression() {
    return equality();
}

Expression Parser::equality() {
    // equality = comparison ( ("שונהמ" | "שווהל") comparison )*
    Expression expression = comparison();

    while(peekMatch({TokenType::NotEquals, TokenType::Equals}))
    {
        Token op = next();
        Expression right = comparison();
        expression = Expression::Binary(expression, op, right);
    }

    return expression;
}

Token Parser::next() {
    Token token = tokens[current];
    current++;
    return token;
}

Token Parser::peek() {
    return tokens[current];
}

bool Parser::peekMatch(initializer_list<TokenType> typesList) {
    vector<TokenType> types = vector<TokenType>(typesList.begin(), typesList.end());
    Token token = peek();

    for (TokenType type : types)
        if (check(type))
            return true;

    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd())
        return false;

    return (peek().type == type);
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}
