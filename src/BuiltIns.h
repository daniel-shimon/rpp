//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#ifndef RPP_RPP_H
#define RPP_RPP_H

#include "Interpreter.h"
#include <initializer_list>
#include <unordered_map>
#include <random>
#include <chrono>

#define IteratorClass "איטרטור"
#define mapAttr (*nativeAttribute<unordered_map<string, Value*>*>(interpreter, "map"))
#define listAttr (*(vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])
#define strAttr (*(string*)getSelf(interpreter)->nativeAttributes["str"])
#define exception(name) {name, new Value(new ClassValue(name, 0, map<string, Value*>(), map<string, Value*>()))}

#define fnc(arity, code) \
    new Value(new NativeFunction(arity, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* { \
        code \
        return nullptr; \
    }))

class RPP {
private:
    static inline InstanceValue* getSelf(Interpreter* interpreter);
    static inline Value* getSelfValue(Interpreter* interpreter);
    template <class T> static inline T nativeAttribute(Interpreter* interpreter, string name);
    static inline void addCls(string name, int initArity, map<string, Value*> staticAttributes, map<string, Value*> methods);
public:
    static void init();
    static mt19937 randEngine;
    static uniform_real_distribution<double> randDist;
};


#endif //RPP_RPP_H
