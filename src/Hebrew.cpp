//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#include "Hebrew.h"

string Hebrew::englishify(string value) {
    string output = "";

    string::iterator it = value.begin();
    while (it != value.end())
    {
        string::iterator prev = it;
        uint32_t ch = utf8::next(it, value.end());
        if (charMap.count(ch) == 0)
            output += string(prev, it);
        else
            output += charMap[ch];
    }
    return output;
}

template<> void Hebrew::print(string value, bool endLine)  {
    pprint(Hebrew::englishify(value));
}

template<> void Hebrew::print(const char* value, bool endLine) {
    pprint(Hebrew::englishify(string(value)));
}
