#ifndef NDEBUG

//
// Created by chen_ on 2022/3/27.
//
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "macro.h"

#ifndef MYCC_SOURCE_AST_UTILS_DEBUG_UTILS_H_
#define MYCC_SOURCE_AST_UTILS_DEBUG_UTILS_H_
namespace Hzcc::AST {
class FunctionDeclNode;
std::string ToString(
    const std::list<
        std::pair<std::string, std::unique_ptr<AST::FunctionDeclNode>>>&
        functions);

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_UTILS_DEBUG_UTILS_H_
#endif
