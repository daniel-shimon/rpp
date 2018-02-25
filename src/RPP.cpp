//
// Created by Dan The Man on 2/21/2018.
//

#include "RPP.h"


void RPP::init() {
    Interpreter::globals.push_back(pair<string, Value*>(
            "רשימה",
            new Value(new ClassValue(map<string, Value*>(), map<string, Value*>
            ({{Init,  new Value(new NativeFunction(-1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                vector<Value *> *list = new vector<Value*>;
                getSelf(interpreter)->nativeAttributes["list"] = list;
                for (Value* value : arguments)
                    list->push_back(value);
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
            {SetItem, new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                (*(vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"])[arguments[0]->getNumber()]
                        = arguments[0];
            }))},
            {"הוצא",  new Value(new NativeFunction(1, []
                    (Interpreter* interpreter, vector<Value*> arguments) -> Value* {
                unsigned int arg = static_cast<unsigned int>(arguments[0]->getNumber());
                vector<Value *> *list = (vector<Value*>*)getSelf(interpreter)->nativeAttributes["list"];
                Value* value = list->at(arg);
                list->erase(list->begin() + arg);
                return value;
            }))},
            }), -1, "רשימה"))));
}

InstanceValue *RPP::getSelf(Interpreter *interpreter) {
    Value* value = interpreter->environment->get(Self);

    if (value == nullptr)
        interpreter->runtimeError("non static method used statically");
    else
        return value->getInstance();
}
