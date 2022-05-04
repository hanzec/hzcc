//
// Created by chen_ on 2022/3/27.
//
#include "debug_utils.h"

#include "AST/statement/function_decl.h"

namespace Hzcc::AST {
std::string ToString(
    const std::list<
        std::pair<std::string, std::unique_ptr<AST::FunctionDeclNode>>>&
        functions) {
    std::string ret;
    for (const auto& function : functions) {
        ret += "\t";
        ret += function.first;
        ret += ": " + function.second->Dump("") + "\n";
    }
    return ret;
}
}  // namespace Hzcc::AST
