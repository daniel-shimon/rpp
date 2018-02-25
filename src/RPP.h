//
// Created by Dan The Man on 2/21/2018.
//

#ifndef RPP_RPP_H
#define RPP_RPP_H

#define IteratorClass "איטרטור"

#include "Interpreter.h"

class RPP {
private:
    static InstanceValue* getSelf(Interpreter* interpreter);
    static Value* getSelfValue(Interpreter* interpreter);
public:
    static void init();
};


#endif //RPP_RPP_H
