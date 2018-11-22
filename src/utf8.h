//
// Created by daniel on 11/20/18.
//

#ifndef RPP_UTF8_H
#define RPP_UTF8_H

#include "utf8/utfcpp.h"
#include "Exception.h"
#include <vector>
#include <string>
#include <utility>

struct T {
    bool moved = false;

    T() {
        printf("empty\n\r");
    }

    T(T &&t) noexcept {
        printf("move rvalue!\n\r");
        t.moved = true;
    }

    T(const T &t) {
        printf("copy? const ref!\n\r");
    }

    T thing(T t) {
        printf("thing\n\r");
        return t;
    }

    void check() {
        printf("moved - %d\n\r", moved);
    }
};

struct Utf8String {
    std::vector<uint32_t> chars;
    string str;

    // empty, copy, move, initializer, begin, end, rbegin, =, +=, length, substring
    Utf8String() = default;

    Utf8String(const Utf8String& s) = default;

    Utf8String(Utf8String &&s) = default;

    inline Utf8String(std::initializer_list<uint32_t> init) { chars.assign(init); }

    inline Utf8String(std::vector<uint32_t>::const_iterator begin, std::vector<uint32_t>::const_iterator end) {
        chars.insert(chars.begin(), begin, end);
    }

    inline Utf8String(unsigned long n, uint32_t chr) { chars.insert(chars.begin(), n, chr); }

    explicit Utf8String(const std::string &str) {
        auto iter = str.begin();
        const auto end = str.end();
        try {
            while (iter < end)
                chars.push_back(utf8::next(iter, end));
            return;
        } catch (utf8::invalid_utf8 &e) {
            invalidUTF8("\"" + string(iter, min(end, iter + 5)) + "\"")
        }
    }

    Utf8String operator+(Utf8String str) const {
        str.chars.insert(str.chars.begin(), chars.begin(), chars.end());
        return str;
    }

    Utf8String &operator=(const Utf8String &s) {
        str = {}; // destroy c_str
        chars = s.chars;
        return *this;
    }

    Utf8String &operator+=(const Utf8String &s) noexcept {
        str = {}; // destroy c_str
        chars.insert(chars.end(), s.chars.begin(), s.chars.end());
        return *this;
    }

    inline Utf8String &operator+=(uint32_t ch) {
        str = {}; // destroy c_str
        chars.push_back(ch);
        return *this;
    }

    inline auto length() { return chars.size(); }

    inline auto begin() const { return chars.cbegin(); }

    inline auto end() const { return chars.end(); }

    inline auto rbegin() { return chars.rbegin(); }

    inline auto rend() { return chars.rend(); }

    inline auto empty() { return chars.empty(); }

    const std::string toString() {
        std::string s;
        auto iter = std::back_inserter(s);
        for (auto ch : chars)
            utf8::append(ch, iter);
        return s;
    }

    typedef decltype(std::string::npos) NType; // type differs with platform
    inline Utf8String substr(NType loc, NType n = std::string::npos) {
        return {chars.begin() + min(loc, chars.size()),
                (n == std::string::npos) ? chars.end() : (chars.begin() + min(loc + n, chars.size()))};
    }

    inline const char *c_str() {
        str = toString(); // make sure lifetime is preserved
        return str.c_str();
    }
};

#endif //RPP_UTF8_H
