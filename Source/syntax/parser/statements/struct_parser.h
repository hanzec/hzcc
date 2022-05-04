//
// Created by chen_ on 2022/3/27.
//
#include "AST/type/StructType.h"
#include "syntax/parser/base_parser.h"
#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_STRUCT_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_STRUCT_PARSER_H_
namespace Hzcc::Syntax::Parser {
class StructDeclare : public ParserBase {
  public:
    StructDeclare() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(AST::CompilationUnit& context,
                                             TokenList& tokens,
                                             TokenList& attributes) override;

  protected:
    static std::shared_ptr<AST::StructType> parse_internal(
        AST::CompilationUnit& context, TokenList& tokens, TokenList& attributes);
};
}  // namespace Hzcc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENTS_STRUCT_PARSER_H_
