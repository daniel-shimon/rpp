//
// Created by daniel on 11/21/18.
//

#include "generic.h"

#if !defined(RPP_UNIX_H) && !defined(Windows)
#define RPP_UNIX_H

#define IO_STR Utf8String
#define IO_STR_LTR_(s) Utf8String(s)
#define IO_STR_LTR(...) IO_STR_LTR_(__VA_ARGS__)
#define IO_CHR uint32_t
#define IO_NL "\n\r"
#include "../Exception.h"
#include "../utf8.h"
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <poll.h>

#define unixError throw RPPException("Unix API Error", "", to_string(errno));

class BaseIO {
    termios oldTerm{}, newTerm{};
    winsize size{};
    int x{}, y{};

public:
    bool enabled;

    void restore();

    vector<string> cmdArguments(int argC, char **argV);

protected:
    BaseIO();

    template<bool ioOnly = false>
    void writeIOStr(IO_STR s) {
        if constexpr (!ioOnly) {
            // calculate location changes
            auto ns = count(s.begin(), s.end(), '\r'); // newlines are always "\n\r"
            long bs;
            if (ns) {
                y += ns;
                ns = 0; // count tail after last line
                for (auto tail = s.rbegin(); *tail != '\r' && tail != s.rend(); tail++)
                    ns++;
                bs = count(s.end()-ns, s.end(), BACKSPACE);
                moveCursor<true>(ns-2*bs);
            } else {
                bs = count(s.begin(), s.end(), BACKSPACE);
                moveCursor<true>(s.length()-2*bs);
            }
        }
        if (write(STDOUT_FILENO, (void *) s.c_str(), (int) s.length()) == -1)
            unixError
    };

    IO_CHR getIOChar();

    void writeIOChar(IO_CHR ch);

    void setCursor(int newX, int newY);

    template<bool dry = false, class X>
    void moveCursor(X dX) {
        if (!dX)
            return;

        x += dX;
        if constexpr (!dry)
            ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
        if (x < 0) {
            x += size.ws_col;
            y--;
        } else if (x >= size.ws_col) {
            x -= size.ws_col;
            y++;
        }
        if constexpr (!dry)
            setLocation();
    };

    template<class X>
    void moveCursorRTL(X dX) {
        // wrap cursor in a right-to-left fashion
        if (!dX)
            return;

        ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
        x += dX;
        if (x < 0) {
            x += size.ws_col;
            y++;
        } else if (x >= size.ws_col) {
            x -= size.ws_col;
            y--;
        }
        setLocation();
    };

    inline IO_STR toIOString(const string &s) {
        return Utf8String(s);
    }

    inline string fromIOStr(IO_STR s) {
        return s.toString();
    }

    inline int windowWidth() {
        return size.ws_col;
    }

    inline int cursorLocation() {
        return x;
    }

    inline int cursorRow() {
        return y;
    }

private:
    string readUntil(char t);

    bool getLocation();

    void setLocation();
};


#endif //RPP_UNIX_H
