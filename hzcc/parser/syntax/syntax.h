//
// Created by chen_ on 2022/12/29.
//

#ifndef HZCC_SYNTAX_H
#define HZCC_SYNTAX_H
#include "ast/CompilationUnit.h"
#include "parser/parser.h"
#include "utils/status/statusor.h"
namespace hzcc::syntax {
StatusOr<std::shared_ptr<ast::CompilationUnit>> Parse(TokenList& tokens);

}  // namespace hzcc::syntax
#endif  // HZCC_SYNTAX_H
