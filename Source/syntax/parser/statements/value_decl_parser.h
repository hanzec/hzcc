//
// Created by chen_ on 2022/3/26.
//
#include "AST/type/Type.h"
#include "syntax/parser/base_parser.h"
#include "syntax/parser/statement_parser.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_VALUE_DECL_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_VALUE_DECL_PARSER_H_
namespace Mycc::Syntax::Parser {
class ValueDeclare : public Statement {
  public:
    ValueDeclare() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(AST::ASTContext& context,
                                             TokenList& tokens,
                                             TokenList& attributes) override;

  private:
};
}  // namespace Mycc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_VALUE_DECL_PARSER_H_
