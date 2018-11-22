//
// Created by daniel on 11/21/18.
//

#include "generic.h"

#if !defined(RPP_WIN_H) && defined(Windows)
#define RPP_WIN_H

#define IO_STR wstring
#define IO_STR_LTR_(s) L##s
#define IO_STR_LTR(...) IO_STR_LTR_(__VA_ARGS__)
#define IO_CHR wchar_t
#define IO_NL "\n"

#include "../Exception.h"
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>

#define windowsError throw RPPException("WinApi Error", "", to_string(GetLastError()));

class BaseIO {
    CONSOLE_SCREEN_BUFFER_INFO cInfo{};
    HANDLE hStdOut, hStdIn;
    DWORD oldMode{};

public:
    bool enabled = false;

    void restore() {
        SetConsoleMode(hStdIn, oldMode);
    }

    vector<string> cmdArguments(int argC, char **_) {
        vector<string> args{};
        args.reserve((uint64_t) argC);
        LPWSTR *cArgs = nullptr;
        {
            volatile auto t = GetCommandLineW();
            if (t == nullptr)
                windowsError
            cArgs = CommandLineToArgvW(t, &argC);
        }
        if (cArgs == nullptr)
            windowsError
        for (auto i = 0; i < argC; i++) {
            args.push_back(fromIOStr(cArgs[i]));
        }
        return args;
    }

protected:
    BaseIO() {
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        if (!GetConsoleMode(hStdIn, &oldMode)) {
            warning("Cannot get console mode. reverting to ascii output.")
            enabled = false;
        } else if (!SetConsoleMode(hStdIn, ENABLE_PROCESSED_INPUT)) {
            warning("Cannot change console mode. reverting to ascii output.")
            enabled = false;
        } else
            enabled = true;
    }

    void writeIOStr(IO_STR s) {
        DWORD out;
        if (!WriteConsoleW(hStdOut, s.c_str(), (DWORD) s.size(), &out, NULL))
            windowsError
        if (out != s.size())
            consoleError("Cannot write " + to_string(s.size()) + " characters")
    }

    IO_CHR getIOChar() {
        IO_CHR ch;
        DWORD read;
        if (!ReadConsoleW(hStdIn, &ch, 1, &read, NULL))
            windowsError
        if (read != 1)
            consoleError("Cannot read character")
        return ch;
    }

    void writeIOChar(IO_CHR ch) {
        DWORD written;
        if (!WriteConsoleW(hStdOut, &ch, 1, &written, NULL))
            windowsError
        if (written != 1)
            consoleError("Cannot write character")
    }

    IO_STR toIOString(string s) {
        auto chars = (size_t) MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int) s.size(), NULL, 0);
        IO_CHR *ws = (IO_CHR *) malloc(chars * sizeof(IO_CHR));
        int converted = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int) s.size(), ws, (int) chars);
        if (converted != chars)
            throw RPPException("Encoding error", "", "cannot convert to UTF16");
        return IO_STR(ws, chars);
    }

    string fromIOStr(IO_STR ws) {
        auto buffSize = (size_t) WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int) ws.size(), NULL, 0, NULL, NULL);
        char *s = (char *) malloc(buffSize);
        int converted = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int) ws.size(), s, (int) buffSize, NULL, NULL);
        if (converted != buffSize)
            throw RPPException("Encoding error", "", "cannot convert to UTF8");
        return string(s, buffSize);
    }

    inline short windowWidth() {
        return cInfo.dwSize.X;
    }

    inline short cursorLocation() {
        GetConsoleScreenBufferInfo(hStdOut, &cInfo);
        return cInfo.dwCursorPosition.X;
    }

    inline short cursorRow() {
        GetConsoleScreenBufferInfo(hStdOut, &cInfo);
        return cInfo.dwCursorPosition.Y;
    }

    template<class X>
    void moveCursor(X x) {
        if (!x)
            return;

        GetConsoleScreenBufferInfo(hStdOut, &cInfo);
        cInfo.dwCursorPosition.X += x;
        if (cInfo.dwCursorPosition.X < 0) {
            cInfo.dwCursorPosition.X += cInfo.dwSize.X;
            cInfo.dwCursorPosition.Y--;
        } else if (cInfo.dwCursorPosition.X >= cInfo.dwSize.X) {
            cInfo.dwCursorPosition.X -= cInfo.dwSize.X;
            cInfo.dwCursorPosition.Y++;
        }
        SetConsoleCursorPosition(hStdOut, cInfo.dwCursorPosition);
    }

    template<class X>
    void moveCursorRTL(X x) {
        // wrap cursor in a right-to-left fashion
        if (!x)
            return;

        GetConsoleScreenBufferInfo(hStdOut, &cInfo);
        cInfo.dwCursorPosition.X += x;
        if (cInfo.dwCursorPosition.X < 0) {
            cInfo.dwCursorPosition.X += cInfo.dwSize.X;
            cInfo.dwCursorPosition.Y++;
        } else if (cInfo.dwCursorPosition.X >= cInfo.dwSize.X) {
            cInfo.dwCursorPosition.X -= cInfo.dwSize.X;
            cInfo.dwCursorPosition.Y--;
        }
        SetConsoleCursorPosition(hStdOut, cInfo.dwCursorPosition);
    }

    void setCursor(short x, short y) {
        GetConsoleScreenBufferInfo(hStdOut, &cInfo);
        cInfo.dwCursorPosition.X = x;
        cInfo.dwCursorPosition.Y = y;
        SetConsoleCursorPosition(hStdOut, cInfo.dwCursorPosition);
    }
};

#endif //RPP_WIN_H
