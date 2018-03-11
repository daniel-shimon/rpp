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

#include "utf8.h"

using namespace std;

class Hebrew {
private:
    static map<uint32_t, string> charMap;
    static map<uint32_t, string> createCharMap();

public:
    static string englishify(string value);
};


#endif //RPP_HEBREW_H
