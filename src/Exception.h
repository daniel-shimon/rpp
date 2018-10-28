//
// Created by Daniel on 10/26/2018.
//

#ifndef RPP_EXCEPTION_H
#define RPP_EXCEPTION_H

#include <string>
#include <exception>

using namespace std;

class RPPException : public exception
{
private:
    string type;
    string signature;
    string message;

public:
    RPPException(string type, string signature, string message = "") : type(type), signature(signature), message(message){};
    virtual const char *what() const throw()
    {
        if (message.empty())
            return (new string(type + " " + signature))->c_str();
        return (new string(type + " " + signature + " : " + message))->c_str();
    };
};

#endif //RPP_EXCEPTION_H
