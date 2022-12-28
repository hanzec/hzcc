//
// Created by chen_ on 2022/5/11.
//
#include <algorithm>
#include <iomanip>
#include <string>
#include <utility>

#include "macro.h"
#ifndef FACT_CLASS_SOURCE_SYNTAX_UTILS_STRING_UTILS_H_
#define FACT_CLASS_SOURCE_SYNTAX_UTILS_STRING_UTILS_H_
namespace hzcc::syntax::utils {
ALWAYS_INLINE std::string &ltrim(std::string &str) {
    auto it2 = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    str.erase(str.begin(), it2);
    return str;
}

ALWAYS_INLINE std::string &rtrim(std::string &str) {
    auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    str.erase(it1.base(), str.end());
    return str;
}

ALWAYS_INLINE void replaceAll(std::string &str, const std::string &from,
                              const std::string &to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();  // In case 'to' contains 'from', like
                                   // replacing 'x' with 'yx'
    }
}

}  // namespace hzcc::syntax
#endif  // FACT_CLASS_SOURCE_SYNTAX_UTILS_STRING_UTILS_H_
