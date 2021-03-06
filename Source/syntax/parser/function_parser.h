//
// Created by chen_ on 2022/3/25.
//

#include <list>

#include "syntax/SyntaxContext.h"
#include "syntax/parser/base_parser.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_FUNCTION_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_FUNCTION_PARSER_H_
namespace Hzcc::Syntax::Parser {
class Function : public ParserBase {
  public:
    Function() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(
        TokenList& tokens, Hzcc::Syntax::SyntaxContext& context) override;
};
}  // namespace Hzcc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_FUNCTION_PARSER_H_
