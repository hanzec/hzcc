//
// Created by chen_ on 2022/3/27.
//
#include "AST/type/struct_type.h"
#include "syntax/parser/base_parser.h"
#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_STRUCT_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_STRUCT_PARSER_H_
namespace Mycc::Syntax::Parser {
class StructDeclare : public ParserBase {
  public:
    StructDeclare() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(AST::ASTContext& context,
                                             TokenList& tokens,
                                             TokenList& attributes) override;

  protected:
    static std::unique_ptr<AST::StructType> parse_internal(
        AST::ASTContext& context, TokenList& tokens, TokenList& attributes);
};
}  // namespace Mycc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_STRUCT_PARSER_H_
