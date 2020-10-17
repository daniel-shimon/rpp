//
// Created by daniel on 11/21/18.
//

#include "Unix.h"

#ifndef Windows

void
BaseIO::restore() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
}

vector<string> BaseIO::cmdArguments(int argC, char **argV) {
    vector<string> args{};
    args.reserve((uint64_t) argC);
    for (auto i = 0; i < argC; i++) {
        args.emplace_back(argV[i]);
    }
    return args;
}

BaseIO::BaseIO() {
    if (!isatty(STDIN_FILENO))
        warning("Not running in a tty. reverting to ascii output.")
    else {
        if (tcgetattr(STDIN_FILENO, &oldTerm) == -1)
            warning("Cannot get console mode. reverting to ascii output.")
        else {
            newTerm = oldTerm;
            cfmakeraw(&newTerm);
            newTerm.c_lflag |= ISIG; // avoid signal handling
            if (tcsetattr(STDIN_FILENO, TCSANOW, &newTerm) == -1)
                warning("Cannot set console mode. reverting to ascii output.")
            else {
                ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
                if (!getLocation())
                    warning("Cannot get cursor location - "
                            "console is not ANSI compliant. reverting to ascii output.")
                else {
                    if (0) {

                    } else {
                        enabled = true;
                        return;
                    }
                }
            }
        }
    }
    restore();
    enabled = false;
}

IO_CHR BaseIO::getIOChar() {
    char buf[4];
    while (true) {
        if (read(STDIN_FILENO, buf, 1) == -1)
            unixError
        switch (buf[0]) {
            case 127: // bypass stupid v100 mapping of Backspace key to Del :(
            case BACKSPACE:
                return BACKSPACE;
            case TAB:
                return TAB;
            case ENTER:
                return ENTER;
            default:
                if ((unsigned char) buf[0] < 32)
                    continue;
                if (utf8::is_valid(buf, buf + 1))
                    return utf8::peek_next(buf, buf + 2);
        }
        for (char i = 1; i < 4; i++) {
            if (read(STDIN_FILENO, buf + i, 1) == -1)
                unixError
            if (utf8::is_valid(buf, buf + i + 1))
                return utf8::peek_next(buf, buf + 4);
        }
        invalidUTF8("console input")
    }
}

void BaseIO::writeIOChar(IO_CHR ch) {
    moveCursor<true>(ch == BACKSPACE ? -1 : 1);
    if (write(STDOUT_FILENO, &ch, 1) == -1)
        unixError
}

void BaseIO::setCursor(int newX, int newY) {
    x = newX;
    if (newY > y)
        writeIOStr<true>(IO_STR((unsigned long) newY - y, '\n') + IO_STR("\r"));
    y = newY;
    setLocation();
}

string BaseIO::readUntil(char t) {
#define size 10
    string s{};
    char buf[size];
    unsigned short i = 0;
    while (true) {
        for (i = 0; i < size; i++) {
            if (read(STDIN_FILENO, buf + i, 1) == -1)
                unixError
            if (*(buf + i) == t) {
                s += string(buf, i);
                return s;
            }
        }
        s += string(buf, size);
    }
#undef size
}

bool BaseIO::getLocation() {
    char tmp;
    pollfd stdIn{};
    stdIn.events = POLLIN;
    stdIn.fd = STDIN_FILENO;
    string before;
    while (poll(&stdIn, 1, 50)) {
        if (read(STDIN_FILENO, &tmp, 1) == -1)
            unixError
        before += tmp;
    }
    if (write(STDOUT_FILENO, "\033[6n", 4) == -1)
        unixError
    if (!poll(&stdIn, 1, 50))
        return false;
    if (read(STDIN_FILENO, &tmp, 1) == -1)
        unixError
    if (read(STDIN_FILENO, &tmp, 1) == -1)
        unixError
    y = stoi(readUntil(';'));
    x = stoi(readUntil('R'));
    if (write(STDIN_FILENO, before.c_str(), before.length()) == -1)
        unixError
    return true;
}

void BaseIO::setLocation() {
    string cmd = "\033[" + to_string(y) + ";" + to_string(x + 1) + "H";
    if (write(STDOUT_FILENO, cmd.c_str(), cmd.length()) == -1)
        unixError
}

#endif
