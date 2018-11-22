//
// Created by Daniel on 10/26/2018.
//

#ifndef RPP_IO_H
#define RPP_IO_H

#include <map>
#include <stack>

#include "io/generic.h"

#ifdef Windows
#include "io/win.h"
#else
#include "io/Unix.h"
#endif

#ifdef ComplexOutput
#define safeExit(c) \
    IO->restore();  \
    exit(c);

// region Whitespace
#define whitespaceCases \
    case IO_CHR('<'): \
    case IO_CHR('>'): \
    case IO_CHR('('):\
    case IO_CHR(')'):\
    case IO_CHR('-'):\
    case IO_CHR('='):\
    case IO_CHR('~'):\
    case IO_CHR('/'):\
    case IO_CHR('['):\
    case IO_CHR(']'):\
    case IO_CHR('\''):\
    case IO_CHR('\\'):\
    case IO_CHR('"'):\
    case IO_CHR('!'):\
    case IO_CHR('+'):\
    case IO_CHR('_'):\
    case IO_CHR(';'):\
    case IO_CHR('*'):\
    case IO_CHR('$'):\
    case IO_CHR('^'):\
    case IO_CHR('&'):
// endregion

struct run {
    IO_STR s = IO_STR_LTR("");
    bool rtl = false;
    unsigned int whitespace = 0;
};

using namespace std;

class __IO : public BaseIO {
    map<IO_CHR, IO_CHR> bidiChars;

    bool isHebrew(IO_CHR ch);

    inline void reverse(IO_STR &from, IO_STR &to);

    void addBidi(const IO_STR &from, IO_STR *to) const;

public:
    __IO();

    string complexInputLTR();

    string complexInputRTL();

    void complexOutputLTR(const string& _s, bool endLine);

    void complexOutputRTL(const string& _s, bool endLine);

    inline void rightAlign() {
        setCursor(windowWidth() - (short) 1, cursorRow());
    };

    inline void leftAlign() {
        setCursor(0, cursorRow());
    };
};

extern __IO *IO;

void IOInit();

#else
#define safeExit(c) exit(c);
#endif

#endif //RPP_IO_H
