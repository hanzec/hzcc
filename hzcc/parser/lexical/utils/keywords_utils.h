//

// Created by chen_ on 2022/6/13.

//

#include <glog/logging.h>

#include <array>

#include "const_msg.h"
#include "macro.h"
#include "options.h"
#include "parser/common/keywords.h"
#include "utils/constexpr_utils.h"

#ifndef MYCC_SOURCE_UTILS_KEYWORDS_UTILS_H_

#define MYCC_SOURCE_UTILS_KEYWORDS_UTILS_H_
//
//namespace hzcc::lexical::KeywordsUtils {
///**
//
// * @Brief: Converting char to ASCII Control Code. There are following ASCII
//
// * Control Code supported:
//
// *         - \0: NUL
//
// *         - \a: BEL
//
// *         - \b: BS
//
// *         - \f: FF
//
// *         - \n: LF
//
// *         - \r: CR
//
// *         - \t: HT
//
// *         - \v: VT
//
// * @param str the char to be converted.
//
// * @return the ASCII Control Code, will return 0xFF if not found.
//
// */
//
//constexpr ALWAYS_INLINE char ToASCIIControlCode(const char str) {
//    switch (str) {
//        case '0':
//
//            return '\0';
//
//        case 'a':
//
//            return '\a';
//
//        case 'b':
//
//            return '\b';
//
//        case 'f':
//
//            return '\f';
//
//        case 't':
//
//            return '\t';
//
//        case 'n':
//
//            return '\n';
//
//        case 'r':
//
//            return '\r';
//
//        case 'v':
//
//            return '\v';
//
//        default:
//
//            return (char)0xFF;
//    }
//}
//
///**
//
// * @Brief: ASCII Control Code to its string('\0' ->"\\0"). There are following
//
// * ASCII Control Code supported:
//
// *         - \0: NUL
//
// *         - \a: BEL
//
// *         - \b: BS
//
// *         - \f: FF
//
// *         - \n: LF
//
// *         - \r: CR
//
// *         - \t: HT
//
// *         - \v: VT
//
// * @param str ascii control code to be converted.
//
// * @return the ASCII Control Code, will return "" if not found.
//
// */
//
//constexpr ALWAYS_INLINE const char *ASCIIControlCodeToString(const char str) {
//    switch (str) {
//        case '\0':
//
//            return "\\0";
//
//        case '\a':
//
//            return "\\a";
//
//        case '\b':
//
//            return "\\b";
//
//        case '\f':
//
//            return "\\f";
//
//        case '\n':
//
//            return "\\n";
//
//        case '\r':
//
//            return "\\r";
//
//        case '\t':
//
//            return "\\t";
//
//        case '\v':
//
//            return "\\v";
//
//        default:
//
//            return "";
//    }
//}
//
//ALWAYS_INLINE std::string TokenTypeToString(uint32_t tokenType) {
//    if (tokenType >= 401 && tokenType <= 500) {
//        return ::hzcc::parser_common::kKeywordTable[tokenType - 401];
//
//    } else if (tokenType >= 351 && tokenType <= 400) {
//        return parser_common::kOperatorTable[tokenType - 351];
//
//    } else if (tokenType <= 0xFF) {
//        return std::string(1, (char)tokenType);  // NOLINT
//
//    } else if (tokenType >= 301 && tokenType <= 306) {
//        return parser_common::kLiteralsSymbol[tokenType - 301];
//
//    } else {
//        LOG(FATAL) << "Unknown token type: " << (int)tokenType;
//    }
//}
//
//}  // namespace hzcc::lexical::KeywordsUtils

#endif  // MYCC_SOURCE_UTILS_KEYWORDS_UTILS_H_