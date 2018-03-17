//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#include "BuiltIns.h"


void RPP::init() {
    // region input

    Interpreter::globals["קלוט"] = new Value(new NativeFunction(-1, [](Interpreter* interpreter,
                                                                      vector<Value*> arguments) -> Value* {
        if (arguments.size())
            interpreter->print(arguments[0], false, false);
        string input;
        getline(cin, input);
        return interpreter->createString(input);
    }));

    // endregion

    // region number

    Interpreter::globals["מספר"] = new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
        if (arguments[0]->type == Number)
            return new Value(arguments[0]->getNumber());
        return new Value(stod(arguments[0]->getString()));
    }));

    // endregion

    // region type

    Interpreter::globals["סוג"] = new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
        return interpreter->createString(arguments[0]->toString());
    }));

    // endregion

    // region random


    Interpreter::globals["אקראי"] = new Value(new NativeFunction(-1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
        double value = RPP::randDist(RPP::randEngine);
        if (arguments.size() == 1)
            return new Value(floor(value * arguments[0]->getNumber()));
        if (arguments.size() == 2)
            return new Value(floor(value * (arguments[1]->getNumber()
                                            - arguments[0]->getNumber()) + arguments[0]->getNumber()));
        return new Value(value);
    }));

    // endregion

    // region exceptions

    Interpreter::globals.insert({
            exception(StopException),
            exception(KeyException),
            exception(IndexException),
                                });

    // endregion

    // region Iterator

    Interpreter::globals[IteratorClass] =
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
                    ({{Init,  new Value(new NativeFunction(-1, []
                            (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                        getSelf(interpreter)->nativeAttributes["list"] = new vector<Value*>(arguments);
                        getSelf(interpreter)->nativeAttributes["i"] = new int(0);
                    }))},
                      {NextItem,  new Value(new NativeFunction(0, []
                            (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                          auto i = *(unsigned int*)getSelf(interpreter)->nativeAttributes["i"];
                          auto list = (vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"];
                          if (i >= list->size())
                              throw interpreter->createInstance(interpreter->globals[StopException], vector<Value*>());
                          *(unsigned int*)getSelf(interpreter)->nativeAttributes["i"] += 1;
                          return list->at(i);
                    }))},
                     }), -1, IteratorClass));

    // endregion

    // region List

    Interpreter::globals["רשימה"] =
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
            ({{Init,  new Value(new NativeFunction(-1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                getSelf(interpreter)->nativeAttributes["list"] = new vector<Value*>(arguments);
            }))},
            {ToString, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                string value = "[";
                for (Value* element : listAttr)
                    value += element->toString(interpreter) + ", ";
                if (value.size() >= 3)
                    value = value.substr(0, value.size() - 2);
                value += "]";
                return interpreter->createString(value);
            }))},
            {"גודל",  new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return new Value((double)listAttr.size());
            }))},
            {"הוסף",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                listAttr.push_back(arguments[0]);
            }))},
            {GetItem, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                unsigned int x = static_cast<unsigned int>(arguments[0]->getNumber());
                if (listAttr.size() <= x)
                    throw interpreter->createInstance(interpreter->globals[IndexException], vector<Value*>());
                return listAttr[x];
            }))},
            {SetItem, new Value(new NativeFunction(2, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                listAttr[arguments[0]->getNumber()] = arguments[1];
            }))},
            {Iterator, new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return interpreter->createInstance(interpreter->globals[IteratorClass], listAttr);
            }))},
            {"הוצא",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                unsigned int arg = static_cast<unsigned int>(arguments[0]->getNumber());
                listAttr.erase(listAttr.begin() + arg);
            }))},
            {"מצא",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                vector<Value *> list = listAttr;
                int value = find_if(list.begin(), list.end(), [&](Value* value) {
                    return interpreter->equalityEvaluation(value, arguments[0]); }) - list.begin();
                if (value >= list.size())
                    value = -1;
                return new Value((double)value);
            }))},
            }), -1, "רשימה"));

    // endregion

    // region String

    Interpreter::globals[StringClass] =
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
            ({{Init,  new Value(new NativeFunction(-1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                if (arguments.size() == 1)
                    if (arguments[0]->isString())
                        getSelf(interpreter)->nativeAttributes["str"] = new string(arguments[0]->getString());
                    else
                        getSelf(interpreter)->nativeAttributes["str"] = new string(arguments[0]->toString(interpreter));
                else
                    getSelf(interpreter)->nativeAttributes["str"] = new string();
            }))},
            {ToString, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return interpreter->createString(strAttr);
            }))},
            {"גודל",  new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return new Value((double)utf8::distance(strAttr.begin(), strAttr.end()));
            }))},
            {GetItem, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                unsigned int x = static_cast<unsigned int>(arguments[0]->getNumber());
                string str = strAttr;
                if (utf8::distance(str.begin(), str.end()) <= x)
                    throw interpreter->createInstance(interpreter->globals[IndexException], vector<Value*>());
                string::iterator start = str.begin();
                utf8::advance(start, x, str.end());
                string::iterator end = start;
                utf8::next(end, str.end());

                return interpreter->createString(string(start, end));
            }))},
            {Iterator, new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                string str = strAttr;
                vector<Value*> chars;
                for (string::iterator it = str.begin(); it < str.end(); utf8::next(it, str.end())) {
                    string::iterator end = it;
                    utf8::next(end, str.end());
                    chars.push_back(interpreter->createString(string(it, end)));
                }
                return interpreter->createInstance(interpreter->globals[IteratorClass], chars);
            }))},
            {"מצא",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                string str = strAttr;
                string needle = arguments[0]->getString();
                int needleLen = utf8::distance(needle.begin(), needle.end());
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

                return new Value((double)value);
            }))},
            }), -1, StringClass));

    // endregion

    // region Dict

    Interpreter::globals["מילון"] =
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
            ({{Init,  new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                getSelf(interpreter)->nativeAttributes["map"] = new unordered_map<string, Value*>();
            }))},
            {ToString, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                string value = "{";
                for (pair<string, Value*> element : mapAttr)
                    value += element.first + ": " + element.second->toString(interpreter) + ", ";
                if (value.size() >= 3)
                    value = value.substr(0, value.size() - 2);
                value += "}";
                return interpreter->createString(value);
            }))},
            {"גודל",  new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return new Value((double)mapAttr.size());
            }))},
            {GetItem, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                const string x = arguments[0]->getString();
                if (!mapAttr.count(x))
                    throw interpreter->createInstance(interpreter->globals[KeyException], vector<Value*>());
                return mapAttr[x];
            }))},
            {SetItem, new Value(new NativeFunction(2, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                mapAttr[arguments[0]->getString()] = arguments[1];
            }))},
            {Iterator, new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                vector<Value*> keys;
                for (pair<string, Value*> element : mapAttr)
                    keys.push_back(interpreter->createString(element.first));
                return interpreter->createInstance(interpreter->globals[IteratorClass], keys);
            }))},
            {"מכיל", new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return new Value((bool)mapAttr.count(arguments[0]->getString()));
            }))},
            {"הוצא",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                mapAttr.erase(arguments[0]->getString());
            }))},
            }), 0, "מילון"));

    // endregion

    // region Range

    Interpreter::globals["טווח"] =
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
                    ({{Init,  new Value(new NativeFunction(-1, []
                            (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                        map<string, void *> &nativeAttributes = getSelf(interpreter)->nativeAttributes;
                        nativeAttributes["i"] = new int(arguments.size() >= 2 ? (int) arguments[0]->getNumber() : 0);
                        nativeAttributes["max"] = new int(arguments.size() >= 2 ? arguments[1]->getNumber() :
                                                          arguments[0]->getNumber());
                    }))},
                      {NextItem,  new Value(new NativeFunction(0, []
                              (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                          auto i = *(int*)getSelf(interpreter)->nativeAttributes["i"];
                          auto max = *(int*)getSelf(interpreter)->nativeAttributes["max"];
                          if (i >= max) {
                              delete &i;
                              delete &max;
                              throw interpreter->createInstance(interpreter->globals[StopException], vector<Value*>());
                          }
                          *(int*)getSelf(interpreter)->nativeAttributes["i"] += 1;
                          return new Value((double)i);
                      }))},
                      {Iterator,  new Value(new NativeFunction(0, []
                              (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                          return getSelfValue(interpreter);
                      }))},
                     }), -1, "טווח"));

    // endregion
}

InstanceValue *RPP::getSelf(Interpreter *interpreter) {
    Value* value = interpreter->environment->get(Self);

    if (value == nullptr)
        interpreter->runtimeError("non static method used statically");
    else
        return value->getInstance();
}

Value *RPP::getSelfValue(Interpreter *interpreter) {
    Value* value = interpreter->environment->get(Self);

    if (value == nullptr)
        interpreter->runtimeError("non static method used statically");
    else
        return value;
}

template<class T>
T RPP::nativeAttribute(Interpreter *interpreter, string name) {
    return (T)getSelf(interpreter)->nativeAttributes[name];
}

mt19937 RPP::randEngine = mt19937((unsigned int) chrono::high_resolution_clock::now()
        .time_since_epoch().count());

uniform_real_distribution<double> RPP::randDist = uniform_real_distribution<double>(0,1);