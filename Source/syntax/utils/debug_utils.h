//
// Created by chen_ on 2022/3/26.
//

#ifndef NDEBUG
#include <list>
#include <string>

#include "lexical/Token.h"
#include "lexical/utils/symbol_utils.h"
#include "lexical/utils/token_utils.h"
#include "macro.h"
#ifndef MYCC_SOURCE_SYNTAX_UTILS_DEBUG_UTILS_H_
#define MYCC_SOURCE_SYNTAX_UTILS_DEBUG_UTILS_H_
namespace Hzcc::Syntax::Debug {
ALWAYS_INLINE std::string PrintAttributeList(
    const std::list<Lexical::Token>& attributes) {
    std::string result = "[";
    for (auto& attribute : attributes) {
        result += attribute.Value() + ",";
    }
    return attributes.empty() ? result += "]"
                              : result.replace(result.size() - 1, 1, "]");
}
}  // namespace Hzcc::Syntax::Debug
#endif  // MYCC_SOURCE_SYNTAX_UTILS_DEBUG_UTILS_H_
#endif
