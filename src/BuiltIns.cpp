//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#include "BuiltIns.h"


void RPP::init() {
    // region input

    Interpreter::globals["קלוט"] =
            fnc(-1,
                if (arguments.size())
                    interpreter->print(arguments[0], false, false);
                        return interpreter->createString(Hebrew::read());
            );

    // endregion

    // region number

    Interpreter::globals["מספר"] =
            fnc(1,
                if (arguments[0]->type == Number)
                    return new Value(arguments[0]->getNumber());
                        return new Value(stod(arguments[0]->getString()));
            );

    // endregion

    // region type

    Interpreter::globals["סוג"] =
            fnc(1,
                return interpreter->createString(arguments[0]->toString());
            );

    // endregion

    // region random


    Interpreter::globals["אקראי"] =
            fnc(-1, {
                double value = RPP::randDist(RPP::randEngine);
                if (arguments.size() == 1)
                    return new Value(
                            floor(value * arguments[0]->getNumber()));
                if (arguments.size() == 2)
                    return new Value(floor(value * (arguments[1]->getNumber()
                                                    -
                                                    arguments[0]->getNumber()) +
                                           arguments[0]->getNumber()));
                return new Value(value);
            });

    // endregion

    // region exceptions

    Interpreter::globals.insert({
                                        exception(StopException),
                                        exception(KeyException),
                                        exception(IndexException),
                                });
    // endregion

    // region Iterator

    addCls(IteratorClass, -1, {}, {
            {Init,
                       fnc(-1,
                           getSelf(interpreter)->nativeAttributes["list"] = new vector<Value *>(
                                   arguments);
                                   getSelf(interpreter)->nativeAttributes["i"] = new int(0);
                       )
            },
            {NextItem, fnc(0,
                           auto i = *(unsigned int *) getSelf(interpreter)->nativeAttributes["i"];
                                   auto list = (vector<Value *> *) getSelf(
                                           interpreter)->nativeAttributes["list"];
                                   if (i >= list->size())
                                       throw interpreter->createInstance(
                                               interpreter->globals[StopException],
                                               vector<Value *>());
                                   *(unsigned int *) getSelf(interpreter)->nativeAttributes["i"] += 1;
                                   return list->at(i);
                       )
            },
    });

// endregion

// region List

    addCls("רשימה", -1, {}, {
            {Init,     fnc(-1,
                           getSelf(interpreter)->nativeAttributes["list"] = new vector<Value *>(arguments);
                       )},
            {ToString, fnc(0,
                           string value = "[";
                                   for (Value *element : listAttr)
                                       value += element->toString(interpreter) + ", ";
                                   if (value.size() >= 3)
                                       value = value.substr(0, value.size() - 2);
                                   value += "]";
                                   return interpreter->createString(value);
                       )},
            {"גודל",   fnc(0,
                           return new Value((double) listAttr.size());
                       )},
            {"הוסף",   fnc(1,
                           listAttr.push_back(arguments[0]);
                       )},
            {GetItem,  fnc(1,
                           unsigned int x = static_cast<unsigned int>(arguments[0]->getNumber());
                                   if (listAttr.size() <= x)
                                       throw interpreter->createInstance(interpreter->globals[IndexException],
                                                                         vector<Value *>());
                                   return listAttr[x];
                       )},
            {SetItem,  fnc(2,
                           listAttr[arguments[0]->getNumber()] = arguments[1];
                       )},
            {Iterator, fnc(0,
                           return interpreter->createInstance(interpreter->globals[IteratorClass], listAttr);
                       )},
            {"הוצא",   fnc(1,
                           unsigned int arg = static_cast<unsigned int>(arguments[0]->getNumber());
                                   listAttr.erase(listAttr.begin() + arg);
                       )},
            {"מצא",    fnc(1, {
                vector<Value *> list = listAttr;
                for (int i = 0; i < list.size(); i++) {
                    if (interpreter->equalityEvaluation(list[i], arguments[0]))
                        return new Value(i);
                }
                return new Value(-1);
            })},
    });

// endregion

// region String

    addCls(StringClass, -1, {},
           {{Init,        fnc(-1, {
               if (arguments.size() == 1)
                   if (arguments[0]->isString())
                       getSelf(interpreter)->nativeAttributes["str"] = new string(
                               arguments[0]->getString());
                   else
                       getSelf(interpreter)->nativeAttributes["str"] = new string(
                               arguments[0]->toString(interpreter));
               else
                   getSelf(interpreter)->nativeAttributes["str"] = new string();
           })},
            {ToString,    fnc(1,
                              return interpreter->createString(strAttr);
                          )},
            {"גודל",      fnc(0,
                              return new Value((double) utf8::distance(strAttr.begin(), strAttr.end()));
                          )},
            {GetItem,     fnc(1, {
                unsigned int x = static_cast<unsigned int>(arguments[0]->getNumber());
                string str = strAttr;
                if (utf8::distance(str.begin(), str.end()) <= x)
                    throw interpreter->createInstance(interpreter->globals[IndexException],
                                                      vector<Value *>());
                string::iterator start = str.begin();
                utf8::advance(start, x, str.end());
                string::iterator end = start;
                utf8::next(end, str.end());

                return interpreter->createString(string(start, end));
            })},
            {Iterator,    fnc(0, {
                string str = strAttr;
                vector<Value *> chars;
                for (string::iterator it = str.begin(); it < str.end(); utf8::next(it, str.end())) {
                    string::iterator end = it;
                    utf8::next(end, str.end());
                    chars.push_back(interpreter->createString(string(it, end)));
                }
                return interpreter->createInstance(interpreter->globals[IteratorClass], chars);
            })},
            {"מצא",       fnc(1, {
                string str = strAttr;
                string needle = arguments[0]->getString();
                auto needleLen = utf8::distance(needle.begin(), needle.end());
                string::iterator searchEnd = str.begin();
                utf8::advance(searchEnd, utf8::distance(str.begin(), str.end()) - needleLen, str.end());

                int value = -1;
                int cursor = 0;
                for (string::iterator it = str.begin(); it <= searchEnd; utf8::next(it, str.end())) {
                    string::iterator itEnd = it;
                    utf8::advance(itEnd, needleLen, str.end());
                    if (string(it, itEnd) == needle) {
                        value = cursor;
                        break;
                    }
                    cursor++;
                }

                return new Value(value);
            })},
            {AddOperator, fnc(1, {
                if (arguments[0]->isString())
                    return interpreter->createString(strAttr + arguments[0]->getString());
                return interpreter->createString(strAttr + arguments[0]->toString(interpreter));
            })},
           });

// endregion

// region Dict

    addCls("מילון", 0, {}, {
            {Init,     fnc(0,
                           (getSelf(interpreter)->nativeAttributes["map"] = new unordered_map<string, Value *>());
                       )},
            {ToString, fnc(1, {
                string value = "{";
                for (pair<string, Value *> element : mapAttr)
                    value += element.first + ": " +
                             element.second->toString(interpreter) + ", ";
                if (value.size() >= 3)
                    value = value.substr(0, value.size() - 2);
                value += "}";
                return interpreter->createString(value);
            })},
            {"גודל",   fnc(0,
                           return new Value((double) mapAttr.size());
                       )},
            {GetItem,  fnc(1, {
                const string x = arguments[0]->getString();
                if (!mapAttr.count(x))
                    throw interpreter->createInstance(
                            interpreter->globals[KeyException],
                            vector<Value *>());
                return mapAttr[x];
            })},
            {SetItem,  fnc(2,
                           mapAttr[arguments[0]->getString()] = arguments[1];
                       )},
            {Iterator, fnc(0, {
                vector<Value *> keys;
                for (pair<string, Value *> element : mapAttr)
                    keys.push_back(interpreter->createString(element.first));
                return interpreter->createInstance(
                        interpreter->globals[IteratorClass], keys);
            })},
            {"מכיל",   fnc(1,
                           return new Value(
                                   (bool) mapAttr.count(arguments[0]->getString()));
                       )},
            {"הוצא",   fnc(1,
                           mapAttr.erase(arguments[0]->getString());
                       )},
    });

// endregion

// region Range
    addCls("טווח", -1, map<string, Value *>(), map<string, Value *>
            ({{Init, new Value(new NativeFunction(-1, []
                    (Interpreter *interpreter, vector<Value *> arguments) -> Value * {
                map<string, void *> &nativeAttributes = getSelf(interpreter)->nativeAttributes;
                nativeAttributes["i"] = new int(arguments.size() >= 2 ? (int) arguments[0]->getNumber() : 0);
                nativeAttributes["max"] = new int(arguments.size() >= 2 ? arguments[1]->getNumber() :
                                                  arguments[0]->getNumber());
                return nullptr;
            }))},
              {NextItem, fnc(0, {
                  auto i = *(int *) getSelf(interpreter)->nativeAttributes["i"];
                  auto max = *(int *) getSelf(interpreter)->nativeAttributes["max"];
                  if (i >= max) {
                      throw interpreter->createInstance(interpreter->globals[StopException], vector<Value *>());
                  }
                  *(int *) getSelf(interpreter)->nativeAttributes["i"] += 1;
                  return new Value((double) i);
              })},
              {Iterator, fnc(0,
                             return getSelfValue(interpreter);
                         )},
             }));


// endregion
}

InstanceValue *RPP::getSelf(Interpreter *interpreter) {
    return getSelfValue(interpreter)->getInstance();
}

Value *RPP::getSelfValue(Interpreter *interpreter) {
    Value *value = interpreter->environment->get(Self);

    if (value == nullptr)
        interpreter->runtimeError("non static method used statically");
    else
        return value;
    return nullptr;
}

template<class T>
T RPP::nativeAttribute(Interpreter *interpreter, string name) {
    return (T) getSelf(interpreter)->nativeAttributes[name];
}

void RPP::addCls(string name, int initArity, map<string, Value *> staticAttributes, map<string, Value *> methods) {
    Interpreter::globals[name] = new Value(new ClassValue(name, initArity, staticAttributes, methods));
}

mt19937 RPP::randEngine = mt19937((unsigned int) chrono::high_resolution_clock::now()
        .time_since_epoch().count());

uniform_real_distribution<double> RPP::randDist = uniform_real_distribution<double>(0, 1);