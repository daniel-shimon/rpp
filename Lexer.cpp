//
// Created by Dan The Man on 1/24/2018.
//
#include "Lexer.h"

Lexer::Lexer(string source)
{
    this->source = source;
    iterator = source.begin();
    end = source.end();
}

vector<Token> Lexer::scanTokens()
{
    while (iterator < end)
    {
        uint32_t ch = next();
        switch (ch)
        {
            case '(': addToken(TokenType::LeftParen); break;
            case ')': addToken(TokenType::RightParen); break;
            case '+': addToken(TokenType::Plus); break;
            case '-': addToken(TokenType::Minus); break;
            case '*': addToken(TokenType::Multiply); break;
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

void Lexer::scanString(char delimiter) {
    string::iterator start = iterator;
    do
    {
        string d = string(start, iterator);
        if (isAtEnd() or next() == '\n')
            throw runtime_error("Unterminated string " + errorSignature());
    }
    while (peek() != delimiter);

    string* value = new string(start, iterator);
    addToken(TokenType::String, value);
    next();
}

bool Lexer::isDigit(uint32_t ch) {
    return '0' <= ch && ch <= '9' ;
}

void Lexer::scanNumber() {
    string::iterator start = iterator;
    utf8::prior(start, source.begin());

    while (isDigit(peek()))
        next();

    if (peek() == '.' && isDigit(peek(1)))
    {
        next();
        while (isDigit(peek()))
            next();
    }

    addToken(TokenType::Number, new double(stod(string(start, iterator))));
}

string Lexer::errorSignature()
{
    return "at line " + to_string(line) + " (" + to_string(index) + ")";
}
