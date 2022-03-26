//
// Created by chen_ on 2022/3/25.
//

#include <list>
#include "utils/type_name_utils.h"
#include "syntax/parser/base_parser.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
namespace Mycc {

namespace Lexical {
class Token;
}
namespace Syntax::Parser {
using namespace TokenListUtils;
class Statement : public ParserBase {
  public:
    Statement();
    std::unique_ptr<AST::ASTNode> parse_impl(                   // NOLINT
        AST::ASTContext& context,                               // NOLINT
        std::list<Lexical::Token>& tokens,                      // NOLINT
        const std::list<Lexical::Token>& attributes) override;  // NOLINT

  protected:
    std::unique_ptr<AST::ASTNode> ParseCommaExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseAssignExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    // TODO: add parser for conditional expression
    std::unique_ptr<AST::ASTNode> ParseConditionalExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalOrExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalAndExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseOrExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseXorExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseAndExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseEqualityExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseRelationalExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseShiftExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseAdditiveExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseMultiplicativeExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParseUnaryExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);

    std::unique_ptr<AST::ASTNode> ParsePostfixExpr(
        AST::ASTContext& context, std::list<Lexical::Token>& tokens);
};
}  // namespace Syntax::Parser
}  // namespace Mycc
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
