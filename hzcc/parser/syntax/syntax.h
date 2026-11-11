//
// Created by chen_ on 2022/12/29.
//

#ifndef HZCC_SYNTAX_H
#define HZCC_SYNTAX_H
#include <memory>

#include "ast/CompilationUnit.h"
#include "parser/parser.h"
#include "utils/status/statusor.h"

namespace hzcc {
namespace ast {
class CompilationUnit;
}  // namespace ast
}  // namespace hzcc

namespace hzcc::syntax {
/**
 * @brief Reserved convenience interface for parsing tokens into a compilation unit.
 * @param tokens Token sequence intended to be consumed by the parser.
 * @return A compilation unit or parsing error when an implementation is provided.
 * @note This declaration currently has no implementation; use GenerateAST instead.
 */
StatusOr<std::shared_ptr<ast::CompilationUnit>> Parse(TokenList& tokens);

}  // namespace hzcc::syntax
#endif  // HZCC_SYNTAX_H
