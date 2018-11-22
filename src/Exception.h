//
// Created by Daniel on 10/26/2018.
//

#ifndef RPP_EXCEPTION_H
#define RPP_EXCEPTION_H

#include <string>
#include <cstring>
#include <iostream>
#include <exception>

#define warning(msg) cout << IO_NL "***" << msg << "***" IO_NL;
#define consoleError(message) throw RPPException("Console Error", "", message);
#define invalidUTF8(signature) throw RPPException("Encoding Error", signature, "Invalid UTF8");

using namespace std;

class RPPException : public exception
{
private:
    string type;
    string signature;
    string message;

public:
    RPPException(string&& type, string&& signature, string message = "") : type(type), signature(signature), message(message){};
    virtual const char *what() const throw()
    {
        string w;
        if (!type.empty())
            w = type;
        if (!signature.empty())
            w += " at " + signature;
        if (!message.empty())
            w += " : " + message;
        auto msg = (char*) malloc(w.size() + 1);
        memcpy(msg, w.c_str(), w.size() + 1);
        return msg;
    };
};

#endif //RPP_EXCEPTION_H
