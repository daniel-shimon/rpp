//
// Created by Dan The Man on 1/24/2018.
//
#include "Lexer.h"

Lexer::Lexer(string* source)
{
    this->source = source;
    start = source->begin();
    iterator = start;
    end = source->end();
}

vector<Token> Lexer::scan()
{
    while (iterator < end)
    {
        uint32_t ch = next();
        switch (ch)
        {
            case '(': addToken(TokenType::RightParen); break;
            case ')': addToken(TokenType::LeftParen); break;
            case '+': addToken(TokenType::Plus); break;
            case '-': addToken(TokenType::Minus); break;
            case '*':
                if (nextMatches('*'))
                    addToken(TokenType::Power);
                else
                    addToken(TokenType::Multiply);
                break;
            case '/':
                if (nextMatches('/'))
                {
                    while (peek() != '\n' && !isAtEnd())
                        next();
                }
                else
                    addToken(TokenType::Divide);
                break;
            case '\r':
            case ' ':
                break;
            case '\n':
                line++;
                index = 1;
                addToken(TokenType::NewLine);
                break;
            case '"':
                scanString();
                break;
            case '\'':
                scanString('\'');
                break;
            default:
                if (isDigit(ch))
                    scanNumber();
                else if (isAlpha(ch))
                    scanIdentifier();
                else
                    throw runtime_error("Unexpected Character " + errorSignature() + " : " + (char)ch);
        }
    }

    addToken(TokenType::EndOfFile);
    return tokens;
}

void Lexer::addToken(TokenType type, void* value) {
    tokens.emplace_back(Token(type, "", value, line, index));
}

bool Lexer::isAtEnd() {
    return iterator == end;
}

uint32_t Lexer::next() {
    return utf8::next(iterator, end);
}

bool Lexer::nextMatches(char ch) {
    if (isAtEnd())
        return false;

    if (utf8::peek_next(iterator, end) != ch)
        return false;

    next();
    return true;
}

uint32_t Lexer::peek(int offset) {
    if (utf8::distance(iterator, end) < offset + 1)
        return '\0';

    string::iterator temp = iterator;
    utf8::advance(temp, offset, end);

    return utf8::peek_next(temp, end);
}

bool Lexer::isDigit(uint32_t ch) {
    return '0' <= ch && ch <= '9' ;
}

void Lexer::scanString(char delimiter) {
    string::iterator start = iterator;
    while (peek() != delimiter)
    {
        if (isAtEnd() or next() == '\n')
            throw runtime_error("Unterminated string " + errorSignature());
    }

    string* value = new string(start, iterator);
    addToken(TokenType::StringLiteral, value);
    next();
}

void Lexer::scanNumber() {
    string::iterator start = iterator;
    utf8::prior(start, this->start);

    while (isDigit(peek()))
        next();

    if (peek() == '.' && isDigit(peek(1)))
    {
        next();
        while (isDigit(peek()))
            next();
    }

    addToken(TokenType::NumberLiteral, new double(stod(string(start, iterator))));
}

void Lexer::scanIdentifier() {
    string::iterator start = iterator;
    utf8::prior(start, this->start);

    while (isAlpha(peek()) || isDigit(peek()))
        next();

    string value = string(start, iterator);

    if (reserved.count(value) == 1)
        addToken(reserved.at(value));
    else
        addToken(TokenType::Identifier, new string(value));
}

string Lexer::errorSignature()
{
    return "at line " + to_string(line) + " (" + to_string(index) + ")";
}

bool Lexer::isAlpha(uint32_t ch) {
    return (1488 <= ch && ch <= 1514) || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}
