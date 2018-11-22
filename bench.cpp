//
// Created by daniel on 11/20/18.
//

#include "src/utf8.h"
#define IO_NL "\n\r"

//#include <stdint.h>

int main() {
    try {
//        T t1, t2;
//        auto k = t1.thing(move(t2));
        {
            auto s = string();
            warning((uintptr_t) s.end().base())
            warning((uintptr_t) s.begin().base())
        }

        auto s = Utf8String();
        warning((uintptr_t)s.end().base())
        warning((uintptr_t)s.begin().base())
    } catch (RPPException& e) {
        printf("%s", e.what());
    }
    return 0;
}
