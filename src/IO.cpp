//
// Created by Daniel on 11/6/2018.
//

#include "IO.h"


#ifdef ComplexOutput
__IO *IO = nullptr;

void IOInit() {
    IO = new __IO();
}

#endif

__IO::__IO() : BaseIO() {
    bidiChars = {
            {IO_CHR('('), IO_CHR(')')},
            {IO_CHR(')'), IO_CHR('(')},
            {IO_CHR('['), IO_CHR(']')},
            {IO_CHR(']'), IO_CHR('[')},
            {IO_CHR('<'), IO_CHR('>')},
            {IO_CHR('>'), IO_CHR('<')},
            {IO_CHR('{'), IO_CHR('}')},
            {IO_CHR('}'), IO_CHR('{')},
    };
}

bool __IO::isHebrew(IO_CHR ch) {
    return ch == L'_' || (1488 <= ch && ch <= 1514);
}

void __IO::reverse(IO_STR &from, IO_STR &to) {
    to = {};
    for (auto pch = from.rbegin(); pch != from.rend(); pch++)
        to += *pch;
}

void __IO::addBidi(const IO_STR &from, IO_STR *to) const {
    for (auto _ch : from)
        if (bidiChars.count(_ch))
            *to += bidiChars.at(_ch);
        else
            *to += _ch;
}

string __IO::complexInputLTR() {
    /*
     * States:
     *  empty - current.s = "", runs size is 0, tmpStr is "", current.rtl does'nt matter.
     *  rtl run - cursor is at end of run ($לכל מי שכאן).
     *  rtl run and whitespace - cursor is at end of txt after whitespace (לכל מי שכאן  $).
     *      also tmpStr contains whitespace characters.
     */
    auto runs = stack<run>();
    bool whitespace = true;
    IO_STR tmpStr = {};
    auto clearAndBack = IO_STR{BACKSPACE, SPACE, BACKSPACE};

    auto current = run();
    current.rtl = false;

    while (IO_CHR ch = getIOChar()) {
        switch (ch) {
            case BACKSPACE:
                if (tmpStr.empty() && current.s.empty() && runs.empty())
                    break;

                if (whitespace) {
                    if (!tmpStr.empty()) { // prevent backspace after hebrew run deletion
                        tmpStr = tmpStr.substr(0, tmpStr.length() - 1);
                        writeIOStr(clearAndBack);
                        if (tmpStr.empty()) {
                            whitespace = false;
                            current.whitespace = 0;
                            if (current.rtl && !current.s.empty())
                                moveCursor(-current.s.length());
                        }
                    }
                } else {
                    current.s = current.s.substr(0, current.s.length() - 1);
                    if (current.rtl) {
                        reverse(current.s, tmpStr);
                        writeIOStr(tmpStr + IO_STR_LTR(" "));
                        moveCursor(-current.s.length() - 1);
                    } else {
                        moveCursor(-1);
                        writeIOChar(SPACE);
                        moveCursor(-1);
                    }
                }

                if (!whitespace && current.s.empty()) { // reached end of current run
                    if (!runs.empty()) {
                        current = runs.top();
                        runs.pop();

                        // prev run ends with whitespace
                        whitespace = true;
                        if (current.whitespace) {
                            tmpStr = current.s.substr(current.s.length() - current.whitespace,
                                                      current.whitespace);
                            current.s = current.s.substr(0, current.s.length() - current.whitespace);
                        } else {
                            tmpStr = {};
                        }
                    } else {
                        // empty state
                        whitespace = true;
                        tmpStr = {};
                    }
                }
                break;
            case ENTER:
                if (!whitespace && current.rtl)
                    moveCursor(current.s.length() + 1);
                setCursor(0, cursorRow() + (short) 1);

                if (whitespace) {
                    if (current.rtl) {
                        runs.push(current);
                        current = run();
                        current.s = tmpStr;
                    } else
                        current.s += tmpStr;
                }

                tmpStr = {};
                runs.push(current);
                while (!runs.empty()) {
                    current = runs.top();
                    if (current.rtl) {
                        IO_STR tmp = {};
                        addBidi(current.s, &tmp);
                        current.s = tmp;
                    }
                    tmpStr = current.s + move(tmpStr);
                    runs.pop();
                }
                writeIOChar(ENTER);
                return fromIOStr(tmpStr);
            case TAB:
                ch = SPACE;
            whitespaceCases
            case SPACE:
                if (!whitespace) {
                    tmpStr = {};
                    if (current.rtl) {
                        moveCursor(current.s.length());
                    }
                }
                tmpStr += ch;
                writeIOChar(ch);
                whitespace = true;
                break;
            default:
                if (whitespace) {
                    current.whitespace = (int) tmpStr.length();
                    if (current.rtl) {
                        if (isHebrew(ch)) {
                            addBidi(tmpStr, &current.s);
                            moveCursor(-current.s.length());
                        } else {
                            runs.push(current);
                            current = run();
                            current.s = tmpStr;
                        }
                    } else {
                        current.s += tmpStr;
                        if (isHebrew(ch)) {
                            runs.push(current);
                            current = run();
                            current.rtl = true;
                        }
                    }
                    whitespace = false;
                }

                current.s += ch;
                if (current.rtl) {
                    reverse(current.s, tmpStr);
                    writeIOStr(tmpStr);
                    moveCursor(-current.s.length());
                } else {
                    writeIOChar(ch);
                }
        }
    }

    return "";
}

string __IO::complexInputRTL() {
    /*
     * States:
     *  empty - current.s = "", runs size is 0, tmpStr is "", current.rtl does'nt matter.
     *  rtl run - cursor is at left end of run ($לכל מי שכאן).
     *  rtl run and whitespace - cursor is at the left of the txt ($    לכל מי שכאן).
     *      also tmpStr contains whitespace characters.
     *  otherwise cursor is at right end of ltr run.
     *  ltr run - cursor is at right side of run (hello$ יאללה מגניב).
     *  ltr run and whitespace - whitespace at left end of run ($   hello שלום וגם).
     */
//    writeIOStr(IO_STR_LTR_("hello?\b\b \b"));
    writeIOStr(IO_STR_LTR_("'ךףכשךדלגגחשדכשףש"));

    auto runs = stack<run>();
    bool whitespace = true;
    IO_STR tmpStr = {};
    auto clearCell = IO_STR{SPACE, BACKSPACE};

    auto current = run();
    current.rtl = true;

    while (IO_CHR ch = getIOChar()) {
        switch (ch) {
            case BACKSPACE:
                if (tmpStr.empty() && current.s.empty() && runs.empty())
                    break;

                if (whitespace) {
                    if (!tmpStr.empty()) { // prevent backspace after run deletion
                        tmpStr = tmpStr.substr(0, tmpStr.length() - 1);
                        moveCursorRTL(1);
                        writeIOStr(clearCell);
                        if (tmpStr.empty()) {
                            whitespace = false;
                            current.whitespace = 0;
                            if (!current.rtl && !current.s.empty())
                                moveCursorRTL(current.s.length());
                        }
                    }
                } else {
                    current.s = current.s.substr(0, current.s.length() - 1);
                    if (current.rtl) {
                        moveCursorRTL(1);
                        writeIOStr(clearCell);
                    } else {
                        if (current.s.length() <= cursorLocation()) {
                            moveCursorRTL(-current.s.length());
                            writeIOStr(IO_STR_LTR(" ") + current.s + IO_STR_LTR("\b"));
                        } else {
                            moveCursorRTL(-current.s.length());
                            volatile auto tailOffset =
                                    current.s.length() - (windowWidth() - cursorLocation()) + 1;
                            writeIOStr(IO_STR_LTR(" ") + current.s.substr(tailOffset) + IO_STR_LTR("\b"));
                            moveCursorRTL(tailOffset);
                        }
                    }
                }

                if (!whitespace && current.s.empty()) { // reached end of current run
                    if (!runs.empty()) {
                        current = runs.top();
                        runs.pop();

                        // prev run ends with whitespace
                        whitespace = true;
                        if (current.whitespace) {
                            tmpStr = current.s.substr(current.s.length() - current.whitespace,
                                                      current.whitespace);
                            current.s = current.s.substr(0, current.s.length() - current.whitespace);
                        } else
                            tmpStr = {};
                    } else {
                        // empty state
                        whitespace = true;
                        tmpStr = {};
                    }
                }
                break;
            case ENTER:
                if (!whitespace && !current.rtl)
                    moveCursorRTL(-current.s.length());
                setCursor(0, cursorRow() + (short) 1);

                if (whitespace)
                    current.s += tmpStr;

                tmpStr = {};
                runs.push(current);
                while (!runs.empty()) {
                    current = runs.top();
                    tmpStr = current.s + move(tmpStr);
                    runs.pop();
                }
                return fromIOStr(tmpStr);
            case TAB:
                ch = SPACE;
            whitespaceCases
            case SPACE:
                if (!whitespace) {
                    tmpStr = {};
                    if (!current.rtl)
                        moveCursorRTL(-current.s.length());
                }
                tmpStr += ch;
                if (bidiChars.count(ch))
                    ch = bidiChars.at(ch);
                if (ch != SPACE)
                    writeIOStr(IO_STR{ch, BACKSPACE});
                moveCursorRTL(-1);
                whitespace = true;
                break;
            default:
                if (whitespace) {
                    current.s += tmpStr;
                    current.whitespace = (int) tmpStr.length();
                    if (current.rtl) {
                        if (!isHebrew(ch)) {
                            runs.push(current);
                            current = run();
                        }
                    } else {
                        if (isHebrew(ch)) {
                            runs.push(current);
                            current = run();
                            current.rtl = true;
                        } else
                            moveCursorRTL(current.s.length());
                    }
                    whitespace = false;
                }

                current.s += ch;
                if (current.rtl) {
                    writeIOStr(IO_STR{ch, BACKSPACE});
                    moveCursorRTL(-1);
                } else {
                    auto loc = cursorLocation();
                    auto row = cursorRow();
                    if (loc + 1 < current.s.length()) {
                        moveCursor(-loc);
                        writeIOStr(current.s.substr(0, (unsigned int) loc));
                        auto i = (unsigned int) (loc + 1);
                        auto j = row;
                        while (i < current.s.length()) { // print lines
                            j++;
                            volatile auto tail = (short) (current.s.length() - i);
                            if (tail <= windowWidth())
                                setCursor(windowWidth() - tail, j);
                            else
                                setCursor(0, j);
                            writeIOStr(current.s.substr(i, (unsigned int) windowWidth()));
                            i += windowWidth();
                        }
                        setCursor(loc, row);
                    } else {
                        moveCursor(1 - current.s.length());
                        writeIOStr(current.s);
                        moveCursor(-1);
                    }
                }
        }
    }

    return "";
}

void __IO::complexOutputLTR(const string &_s, bool endLine) {
    IO_STR s = toIOString(_s);
    auto runs = stack<IO_STR *>();
    auto current = new IO_STR();
    bool whitespace = true;
    bool rtl = false;
    IO_STR tmpStr;

    for (auto pch = s.begin(); pch < s.end(); pch++) {
        auto ch = *pch;
        switch (ch) {
            case ENTER:
            case TAB:
            whitespaceCases
            case SPACE:
                whitespace = true;
                tmpStr += ch;
                break;
            default:
                if (whitespace) {
                    if (rtl) {
                        if (!isHebrew(ch)) {
                            IO_STR rev;
                            reverse(*current, rev);
                            runs.push(new IO_STR(rev));
                            *current = tmpStr;
                            rtl = false;
                        } else
                            addBidi(tmpStr, current);
                    } else {
                        *current += tmpStr;
                        if (isHebrew(ch)) {
                            runs.push(current);
                            current = new IO_STR();
                            rtl = true;
                        }
                    }
                    tmpStr = {};
                    whitespace = false;
                }

                *current += ch;
        }
    }

    if (whitespace) {
        if (rtl) {
            IO_STR tmp;
            reverse(*current, tmp);
            runs.push(new IO_STR(tmp));
            *current = tmpStr;
            rtl = false;
        } else
            *current += tmpStr;
    }

    if (rtl) {
        reverse(*current, tmpStr);
        delete current;
        runs.push(new IO_STR(tmpStr));
    } else
        runs.push(current);

    tmpStr = {};
    while (!runs.empty()) {
        current = runs.top();
        tmpStr = *current + move(tmpStr);
        delete current;
        runs.pop();
    }
    if (endLine)
        tmpStr += IO_STR_LTR(IO_NL);
    writeIOStr(tmpStr);
}

void __IO::complexOutputRTL(const string &_s, bool endLine) {
    IO_STR s = toIOString(_s);
    auto runs = stack<IO_STR *>();
    auto current = new IO_STR();
    bool whitespace = true;
    IO_STR tmpStr = {};
    bool rtl = true;

    for (auto pch = s.begin(); pch < s.end(); pch++) {
        auto ch = *pch;
        switch (ch) {
            case ENTER:
            case TAB:
            whitespaceCases
            case SPACE:
                whitespace = true;
                tmpStr += ch;
                break;
            default:
                if (whitespace) {
                    if (rtl) {
                        addBidi(tmpStr, current);

                        if (!isHebrew(ch)) {
                            reverse(*current, tmpStr);
                            runs.push(new IO_STR(tmpStr));
                            *current = {};
                            rtl = false;
                        }
                    } else {
                        if (isHebrew(ch)) {
                            runs.push(current);
                            current = new IO_STR();
                            addBidi(tmpStr, current);
                            rtl = true;
                        } else
                            *current += tmpStr;
                    }
                    tmpStr = {};
                    whitespace = false;
                }

                *current += ch;
        }
    }

    if (whitespace) {
        if (rtl)
            addBidi(tmpStr, current);
        else {
            runs.push(current);
            current = new IO_STR();
            addBidi(tmpStr, current);
            rtl = true;
        }
    }

    if (rtl) {
        reverse(*current, tmpStr);
        delete current;
        runs.push(new IO_STR(tmpStr));
    } else
        runs.push(current);

    tmpStr = {};
    while (!runs.empty()) {
        current = runs.top();
        tmpStr += *current;
        delete current;
        runs.pop();
    }

    if (tmpStr.length() > cursorLocation()) {
        auto nextLines = tmpStr.length() - cursorLocation();
        // position for str ending
        setCursor((short) 1 + windowWidth() - (short) (nextLines % windowWidth()),
                  cursorRow() + (short) 1 + (short) nextLines / windowWidth());

        // write last line
        writeIOStr(tmpStr.substr(0, tmpStr.length() % windowWidth()));
        tmpStr = tmpStr.substr(tmpStr.length() % windowWidth());
        setCursor(0, cursorRow() - (short) 2);

        while (tmpStr.length() > windowWidth()) {
            writeIOStr(tmpStr.substr(0, (unsigned int) windowWidth()));
            tmpStr = tmpStr.substr((unsigned int) windowWidth());
            setCursor(0, cursorRow() - (short) 2);
        }

        // write first line
        writeIOStr(tmpStr);
        setCursor(windowWidth() - (short) (nextLines % windowWidth()),
                  cursorRow() + (short) nextLines / windowWidth());
    } else {
        moveCursor(-tmpStr.length() + 1);
        writeIOStr(tmpStr);
        moveCursor(-tmpStr.length());
    }
    if (endLine)
        setCursor(0, cursorRow() + (short) 1);
    else
        moveCursorRTL(-1);
}
