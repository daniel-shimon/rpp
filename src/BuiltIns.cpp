//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#include "BuiltIns.h"


void RPP::init() {
    // region input

    Interpreter::globals["קלוט"] = new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
        interpreter->print(arguments[0], false, false);
        string input;
        getline(cin, input);
        return new Value(input);
    }));

    // endregion

    // region str

    Interpreter::globals["טקסט"] = new Value(new NativeFunction(1, [](Interpreter* interpreter, vector<Value*> arguments) -> Value* {
        return new Value(arguments[0]->toString(interpreter));
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
        return new Value(arguments[0]->toString());
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
                              throw interpreter->createInstance(interpreter->globals[StopException], nullptr,
                                                                vector<Value*>());
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
                return new Value(value);
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
                if (listAttr.size() >= x)
                    throw interpreter->createInstance(interpreter->globals[IndexException], nullptr,
                                                            vector<Value*>());
                return listAttr[x];
            }))},
            {SetItem, new Value(new NativeFunction(2, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                listAttr[arguments[0]->getNumber()] = arguments[1];
            }))},
            {Iterator, new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return interpreter->createInstance(interpreter->globals[IteratorClass], nullptr, listAttr);
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
                return new Value(value);
            }))},
            {"גודל",  new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return new Value((double)mapAttr.size());
            }))},
            {GetItem, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                const string x = arguments[0]->getString();
                if (!mapAttr.count(x))
                    throw interpreter->createInstance(interpreter->globals[KeyException], nullptr,
                                                            vector<Value*>());
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
                    keys.push_back(new Value(element.first));
                return interpreter->createInstance(interpreter->globals[IteratorClass], nullptr, keys);
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
                        nativeAttributes["max"] = arguments.size() >= 2 ? arguments[1] : arguments[0];
                    }))},
                      {NextItem,  new Value(new NativeFunction(0, []
                              (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                          auto i = *(int*)getSelf(interpreter)->nativeAttributes["i"];
                          double max = ((Value*)getSelf(interpreter)->nativeAttributes["max"])->getNumber();
                          if (i >= max)
                              throw interpreter->createInstance(interpreter->globals[StopException], nullptr,
                                                                vector<Value*>());
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
