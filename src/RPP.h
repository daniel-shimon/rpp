//
// Created by Dan The Man on 2/21/2018.
//

#ifndef RPP_RPP_H
#define RPP_RPP_H

#include "Interpreter.h"

class RPP {
private:
    static InstanceValue* getSelf(Interpreter* interpreter);
public:
    static void init();
};


#endif //RPP_RPP_H
