//
// Created by Dan The Man on 2/21/2018.
//

#include "RPP.h"


void RPP::init() {
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
                for (Value* element : *(vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])
                    value += element->toString(interpreter) + ", ";
                if (value.size() >= 3)
                    value = value.substr(0, value.size() - 2);
                value += "]";
                return new Value(value);
            }))},
            {"גודל",  new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return new Value((double)((vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])->size());
            }))},
            {"הוסף",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                ((vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])->push_back(arguments[0]);
            }))},
            {GetItem, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return ((vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])->at(
                        static_cast<unsigned int>(arguments[0]->getNumber()));
            }))},
            {SetItem, new Value(new NativeFunction(2, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                (*(vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])[arguments[0]->getNumber()]
                        = arguments[0];
            }))},
            {Iterator, new Value(new NativeFunction(0, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                return interpreter->createInstance(interpreter->globals[IteratorClass], nullptr,
                                                   *(vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"]);
            }))},
            {"הוצא",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                unsigned int arg = static_cast<unsigned int>(arguments[0]->getNumber());
                vector<Value *> *list = (vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"];
                Value* value = list->at(arg);
                list->erase(list->begin() + arg);
                return value;
            }))},
            }), -1, "רשימה"));

    // endregion

    // region range

    Interpreter::globals["טווח"] =
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
                    ({{Init,  new Value(new NativeFunction(1, []
                            (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                        getSelf(interpreter)->nativeAttributes["i"] = new int(0);
                        getSelf(interpreter)->nativeAttributes["max"] = arguments[0];
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
