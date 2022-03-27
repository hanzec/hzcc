//
// Created by chen_ on 2022/3/25.
//

#include <list>

#include "syntax/parser/base_parser.h"
#include "utils/type_name_utils.h"

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
    Statement() noexcept;
    std::unique_ptr<AST::ASTNode> parse_impl(AST::ASTContext& context,
                                             TokenList& tokens,
                                             TokenList& attributes) override;

  protected:
    Statement(size_t id, std::string_view ASTNodeName) noexcept
        : ParserBase(id, ASTNodeName){};

    // TODO should be all static function
    std::unique_ptr<AST::ASTNode> ParseCommaExpr(AST::ASTContext& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAssignExpr(AST::ASTContext& context,
                                                  TokenList& tokens);

    // TODO: add parser for conditional expression
    std::unique_ptr<AST::ASTNode> ParseConditionalExpr(AST::ASTContext& context,
                                                       TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalOrExpr(AST::ASTContext& context,
                                                     TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalAndExpr(AST::ASTContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseOrExpr(AST::ASTContext& context,
                                                     TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseXorExpr(AST::ASTContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseAndExpr(AST::ASTContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseEqualityExpr(AST::ASTContext& context,
                                                    TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseRelationalExpr(AST::ASTContext& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseShiftExpr(AST::ASTContext& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAdditiveExpr(AST::ASTContext& context,
                                                    TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseMultiplicativeExpr(
        AST::ASTContext& context, TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseUnaryExpr(AST::ASTContext& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAccessExpr(AST::ASTContext& context,
                                                  TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParsePostfixExpr(AST::ASTContext& context,
                                                   TokenList& tokens);
};
}  // namespace Syntax::Parser
}  // namespace Mycc
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
