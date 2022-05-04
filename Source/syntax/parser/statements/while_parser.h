//
// Created by chen_ on 2022/3/25.
//
#include "syntax/parser/base_parser.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_WHILE_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_WHILE_PARSER_H_
namespace Hzcc::Syntax::Parser {
class WhileStatement : public ParserBase {
  public:
    WhileStatement() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(AST::CompilationUnit& context,
                                             TokenList& tokens) override;
};

}  // namespace Hzcc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_WHILE_PARSER_H_
