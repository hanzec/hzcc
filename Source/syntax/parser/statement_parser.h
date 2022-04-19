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
    std::unique_ptr<AST::ASTNode> parse_impl(AST::CompilationUnit& context,
                                             TokenList& tokens,
                                             TokenList& attributes) override;

  protected:
    Statement(size_t id, std::string_view ASTNodeName) noexcept
        : ParserBase(id, ASTNodeName){};

    // TODO should be all static function
    std::unique_ptr<AST::ASTNode> ParseCommaExpr(AST::CompilationUnit& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAssignExpr(AST::CompilationUnit& context,
                                                  TokenList& tokens);

    // TODO: add parser for conditional expression
    std::unique_ptr<AST::ASTNode> ParseConditionalExpr(AST::CompilationUnit& context,
                                                       TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalOrExpr(AST::CompilationUnit& context,
                                                     TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseLogicalAndExpr(AST::CompilationUnit& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseOrExpr(AST::CompilationUnit& context,
                                                     TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseXorExpr(AST::CompilationUnit& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseBitwiseAndExpr(AST::CompilationUnit& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseEqualityExpr(AST::CompilationUnit& context,
                                                    TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseRelationalExpr(AST::CompilationUnit& context,
                                                      TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseShiftExpr(AST::CompilationUnit& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAdditiveExpr(AST::CompilationUnit& context,
                                                    TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseMultiplicativeExpr(
        AST::CompilationUnit& context, TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseUnaryExpr(AST::CompilationUnit& context,
                                                 TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParseAccessExpr(AST::CompilationUnit& context,
                                                  TokenList& tokens);

    std::unique_ptr<AST::ASTNode> ParsePostfixExpr(AST::CompilationUnit& context,
                                                   TokenList& tokens);
};
}  // namespace Syntax::Parser
}  // namespace Mycc
#endif  // MYCC_SOURCE_SYNTAX_PARSER_STATEMENT_PARSER_H_
