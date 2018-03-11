//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#ifndef RPP_RPP_H
#define RPP_RPP_H

#include "Interpreter.h"
#include <unordered_map>

#define IteratorClass "איטרטור"
#define mapAttr (*nativeAttribute<unordered_map<string, Value*>*>(interpreter, "map"))
#define listAttr (*(vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])
#define exception(name) {name, new Value(new ClassValue(map<string, Value*>(), map<string, Value*>(), 0, name))}

class RPP {
private:
    static InstanceValue* getSelf(Interpreter* interpreter);
    static inline Value* getSelfValue(Interpreter* interpreter);
    template <class T> inline static T nativeAttribute(Interpreter* interpreter, string name);
public:
    static void init();
};


#endif //RPP_RPP_H
