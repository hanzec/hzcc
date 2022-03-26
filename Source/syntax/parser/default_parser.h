//
// Created by chen_ on 2022/3/25.
//

#include "AST/ast_node.h"
#include "base_parser.h"
#include "utils/logging.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_DEFAULT_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_DEFAULT_PARSER_H_
namespace Mycc::Syntax::Parser {
class Default : public ParserBase {
  public:
    explicit Default(std::string_view& parser_name)
        : ParserBase(0,parser_name), _name(parser_name) {}

    std::unique_ptr<AST::ASTNode> parse_impl(              // NOLINT
        AST::ASTContext& context,                          // NOLINT
        std::list<Lexical::Token>& tokens,                 // NOLINT
        const std::list<Lexical::Token>& attributes) override {  // NOLINT

        LOG(ERROR) << "Parser " << _name << " not implemented";
        return nullptr;
    }

  private:
    std::string_view _name;
};
}  // namespace Mycc::Syntax::Parser
#endif  // MYCC_SOURCE_SYNTAX_PARSER_DEFAULT_PARSER_H_
