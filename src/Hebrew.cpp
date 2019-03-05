//
// Created by Daniel Shimon on 2/21/2018.
// daielk09@gmail.com
//

#include "Hebrew.h"

string Hebrew::englishify(string value) {
    string output = "";

    string::iterator it = value.begin();
    while (it != value.end()) {
        string::iterator prev = it;
        uint32_t ch = utf8::next(it, value.end());
        if (charMap.count(ch) == 0)
            output += string(prev, it);
        else
            output += charMap[ch];
    }
    return output;
}

void Hebrew::print(const string& value, bool endLine, bool rtl) {
#ifdef ComplexOutput
    if (IO->enabled) {
        if (rtl) {
            IO->rightAlign();
            IO->complexOutputRTL(value, endLine);
        } else {
            IO->leftAlign();
            IO->complexOutputLTR(value, endLine);
        }
        return;
    }
#endif
    pprint(Hebrew::englishify(value));
}

void Hebrew::print(const char *value, bool endLine, bool rtl) {
    print(string(value), endLine, rtl);
}

string Hebrew::read(bool rtl) {
#ifdef ComplexOutput
    if (IO->enabled)
        if (rtl)
            return IO->complexInputRTL();
        else
            return IO->complexInputLTR();
#endif
    string input;
    getline(cin, input);
    return input;
}
