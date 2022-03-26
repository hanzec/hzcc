//
// Created by chen_ on 2022/3/25.
//
#include "syntax/parser/base_parser.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_DO_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_DO_PARSER_H_
namespace Mycc::Syntax::Parser {
class DoStatement : public ParserBase {
  public:
    DoStatement();
    std::unique_ptr<AST::ASTNode> parse_impl(  // NOLINT
        AST::ASTContext& context,              // NOLINT
        std::list<Lexical::Token>& tokens) override;
};
}  // namespace Mycc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_DO_PARSER_H_
