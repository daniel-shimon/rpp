//
// Created by Daniel Shimon on 1/24/2018.
// daielk09@gmail.com
//
#include "Lexer.h"

Lexer::Lexer(string&& source)
{
    this->source = source;
    iterator = this->source.begin();
    end = this->source.end();
}

vector<shared_ptr<Token>> Lexer::scan() {
    try {
        return _scan();
    } catch (utf8::exception& e) {
        invalidUTF8(Token::errorSignature(line, index - 1))
    }
}

inline vector<shared_ptr<Token>> Lexer::_scan() {
    while (iterator < end)
    {
        uint32_t ch = next();
        switch (ch)
        {
            case '(': addToken(OpenParen, "("); break;
            case ')': addToken(CloseParen, ")"); break;
            case '[': addToken(OpenSquare, "["); break;
            case ']': addToken(CloseSquare, "]"); break;
            case '+': addToken(Plus, "+"); break;
            case '-': addToken(Minus, "-"); break;
            case '%': addToken(Modulo, "%"); break;
            case ';': addToken(Semicolon, ";"); break;
            case ':': addToken(Colon, ":"); break;
            case ',': addToken(Comma, ","); break;
            case '.': addToken(Dot, "."); break;
            case '!':
                if (nextMatches('='))
                    addToken(NotEquals, "!=");
                else
                    addToken(Not, "!");
                break;
            case '*':
                if (nextMatches('*'))
                    addToken(Power, "**");
                else
                    addToken(Multiply, "*");
                break;
            case '/':
                if (nextMatches('/'))
                {
                    while (peek() != '\n' && !isAtEnd())
                        next();
                }
                else
                    addToken(Divide, "/");
                break;
            case '>':
                if (nextMatches('='))
                    addToken(BiggerEq, ">=");
                else
                    addToken(Bigger, ">");
                break;
            case '<':
                if (nextMatches('='))
                    addToken(SmallerEq, "<=");
                else
                    addToken(Smaller, "<");
                break;
            case '=':
                if (nextMatches('='))
                    addToken(Equals, "==");
                else
                    addToken(Assign, "=");
                break;
            case ' ':
                if (peek() == ' ' && peek(1) == ' ' && peek(2) == ' ') {
                    next();
                    next();
                    next();
                }
                else
                    break;
            case '\t': addToken(Indent, "tab"); break;
            case '\r':
            case '\n':
                addToken(NewLine, "newline");
                line++;
                index = 1;
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
                else if (isAlpha(ch) || ch == '_')
                    scanIdentifier();
                else
                    throw RPPException("Unexpected character", Token::errorSignature(line, index), string(1, (char)ch));
        }
    }

    addToken(EndOfFile, "");
    return tokens;
}

void Lexer::addToken(TokenType type, string lexeme, void* value) {
    tokens.push_back(make_shared<Token>(type, lexeme, value, line, index));
}

bool Lexer::isAtEnd() {
    return iterator == end;
}

uint32_t Lexer::next() {
    index++;
    if (utf8::is_bom(iterator))
        iterator += 3;
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
            throw RPPException("Unterminated string", Token::errorSignature(line, index, string(start, iterator)));
    }

    string* value = new string(start, iterator);
    addToken(StringLiteral, *value, value);
    next();
}

void Lexer::scanNumber() {
    string::iterator start = iterator;
    utf8::prior(start, this->source.begin());

    while (isDigit(peek()))
        next();

    if (peek() == '.' && isDigit(peek(1)))
    {
        next();
        while (isDigit(peek()))
            next();
    }

    string numberString = string(start, iterator);
    addToken(NumberLiteral, numberString, new double(stod(numberString)));
}

void Lexer::scanIdentifier() {
    string::iterator start = iterator;
    utf8::prior(start, this->source.begin());

    while (isAlpha(peek()) || isDigit(peek()) || peek() == '_')
        next();

    string value = string(start, iterator);

    if (reserved.count(value) == 1)
        addToken(reserved.at(value), value);
    else
        addToken(Identifier, value, new string(value));
}

Token::~Token() {
    switch (this->type) {
        case NumberLiteral:
            delete (double*)this->value;
            break;
        case StringLiteral:
        case Identifier:
            delete (string*)this->value;
        default:
            return;
    }
}

string Token::errorSignature() {
    return Token::errorSignature(line, index, lexeme);
}

string Token::errorSignature(int line, int index, string lexeme) {
    if (lexeme.size() > 0)
        return "line " + to_string(line) + " index " + to_string(index) + " ('" + lexeme + "')";
    return "line " + to_string(line) + " index " + to_string(index);
}

bool Lexer::isAlpha(uint32_t ch) {
    return (1488 <= ch && ch <= 1514) || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}
