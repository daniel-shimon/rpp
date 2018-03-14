//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#ifndef RPP_HEBREW_H
#define RPP_HEBREW_H

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <algorithm>
#include <iostream>

#include "utf8.h"
#define Self "אני"
#define Init "__התחל__"
#define ToString "__טקסט__"
#define GetItem "__קח__"
#define SetItem "__שים__"
#define NextItem "__הבא__"
#define Iterator "__איטרטור__"
#define StopException "__שגיאת_עצירה__"
#define KeyException "__שגיאת_מפתח__"
#define IndexException "__שגיאת_מיקום__"
#define StringClass "טקסט"
#define AddOperator "__חיבור__"

#define pprint(x) cout << x; if (endLine) cout << endl;

using namespace std;

namespace Hebrew {
    namespace {
        map<uint32_t, string> createCharMap() {
            string alphabet = "אבגדהוזחטיכלמנסעפצקרשתםףץןך";
            string::iterator it = alphabet.begin();

            map<uint32_t, string> hebrew;
            hebrew[utf8::next(it, alphabet.end())] = "a";
            hebrew[utf8::next(it, alphabet.end())] = "b";
            hebrew[utf8::next(it, alphabet.end())] = "g";
            hebrew[utf8::next(it, alphabet.end())] = "d";
            hebrew[utf8::next(it, alphabet.end())] = "h";
            hebrew[utf8::next(it, alphabet.end())] = "w";
            hebrew[utf8::next(it, alphabet.end())] = "z";
            hebrew[utf8::next(it, alphabet.end())] = "j";
            hebrew[utf8::next(it, alphabet.end())] = "t";
            hebrew[utf8::next(it, alphabet.end())] = "y";
            hebrew[utf8::next(it, alphabet.end())] = "c";
            hebrew[utf8::next(it, alphabet.end())] = "l";
            hebrew[utf8::next(it, alphabet.end())] = "m";
            hebrew[utf8::next(it, alphabet.end())] = "n";
            hebrew[utf8::next(it, alphabet.end())] = "s";
            hebrew[utf8::next(it, alphabet.end())] = "'a";
            hebrew[utf8::next(it, alphabet.end())] = "p";
            hebrew[utf8::next(it, alphabet.end())] = "tz";
            hebrew[utf8::next(it, alphabet.end())] = "k";
            hebrew[utf8::next(it, alphabet.end())] = "r";
            hebrew[utf8::next(it, alphabet.end())] = "sh";
            hebrew[utf8::next(it, alphabet.end())] = "t";
            hebrew[utf8::next(it, alphabet.end())] = "m";
            hebrew[utf8::next(it, alphabet.end())] = "f";
            hebrew[utf8::next(it, alphabet.end())] = "tz";
            hebrew[utf8::next(it, alphabet.end())] = "n";
            hebrew[utf8::next(it, alphabet.end())] = "j";

            return hebrew;
        };
        map<uint32_t, string> charMap = createCharMap();
    }

    string englishify(string value);
    template <class T> void print(T value, bool endLine = false) {
        pprint(value);
    };
    template<> void print(string value, bool endLine);
    template<> void print(const char* value, bool endLine);
};

#endif //RPP_HEBREW_H
