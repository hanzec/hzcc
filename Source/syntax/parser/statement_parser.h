//
// Created by chen_ on 2022/3/25.
//

#include <list>

#include "syntax/SyntaxContext.h"
#include "syntax/parser/base_parser.h"
#include "utils/type_name_utils.h"

#ifndef MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
#define MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
namespace Hzcc {

namespace Lexical {
class Token;
}
namespace Syntax::Parser {
class Statement : public ParserBase {
  public:
    Statement() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(
        TokenList& tokens, Hzcc::Syntax::SyntaxContext& context) override;

  protected:
    Statement(size_t id, std::string_view ASTNodeName) noexcept
        : ParserBase(id, ASTNodeName){};

    // TODO should be all static function
    std::unique_ptr<AST::ASTNode> ParseCommaExpr(SyntaxContext& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAssignExpr(SyntaxContext& context,
                                                  TokenList& tokens);

    // TODO: add parser for conditional expression
    std::unique_ptr<AST::ASTNode> ParseConditionalExpr(SyntaxContext& context,
                                                       TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalOrExpr(SyntaxContext& context,
                                                     TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalAndExpr(SyntaxContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseOrExpr(SyntaxContext& context,
                                                     TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseXorExpr(SyntaxContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseAndExpr(SyntaxContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseEqualityExpr(SyntaxContext& context,
                                                    TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseRelationalExpr(SyntaxContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseShiftExpr(SyntaxContext& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAdditiveExpr(SyntaxContext& context,
                                                    TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseMultiplicativeExpr(
        SyntaxContext& context, TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseUnaryExpr(SyntaxContext& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAccessExpr(SyntaxContext& context,
                                                  TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParsePostfixExpr(SyntaxContext& context,
                                                   TokenList& tokens);
};
}  // namespace Syntax::Parser
}  // namespace Hzcc
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
