#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "ast/expr/Expr.h"
#include "options.h"
#include "parser/syntax/utils/common_utils.h"
#include "utils/status/status.h"
#ifndef MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#define MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
namespace hzcc::syntax::parser {
class ParserBase {
  public:
    ParserBase(unsigned long id, std::string_view name) noexcept
        : _id(id), _astNodeName(std::move(name)) {}

    virtual ~ParserBase() = default;

    StatusOr<ast::StmtPtr> parse(TokenList& token, Ctx& context) {
        return parse_impl(token, context);
    };

    [[nodiscard]] size_t ParserID() const { return _id; }

    [[nodiscard]] std::string_view GetASTNodeName() const {
        return _astNodeName;
    }

  protected:
    virtual StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens,
                                              hzcc::syntax::Ctx& context) = 0;

    static bool WithinLoop() { return _inside_loop; }
    static bool ExitLoop() { return _inside_loop = false; }
    static bool EnterLoop() { return _inside_loop = true; }

  private:
    const size_t _id = -1;
    const std::string _astNodeName;
    inline static bool _inside_loop = false;
};

class DeclStatement : public ParserBase {
  public:
    explicit DeclStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens,
                                      hzcc::syntax::Ctx& context) override;

  protected:
    using RecordPtr = std::unique_ptr<ast::RecordDecl>;

    static StatusOr<RecordPtr> parse_struct(TokenList& tokens,
                                            hzcc::syntax::Ctx& context);

    static StatusOr<ast::StmtPtr> parse_var(TokenList& tokens,
                                            ast::TypePtr base_type,
                                            ast::TypePtr curr_type,
                                            hzcc::syntax::Ctx& context);
    static StatusOr<ast::StmtPtr> parse_func(TokenList& tokens,
                                             ast::TypePtr return_type,
                                             hzcc::syntax::Ctx& context);

  private:
    bool _is_fist_declare = true;
    uint_fast32_t attribute_size_ = 0;
};

class CompoundStatement : public ParserBase {
  public:
    explicit CompoundStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens, Ctx& context) override;
};

class DoStatement : public ParserBase {
  public:
    explicit DoStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens, Ctx& context) override;
};

class ForStatement : public ParserBase {
  public:
    explicit ForStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens, Ctx& context) override;
};

class IfStatement : public ParserBase {
  public:
    explicit IfStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens, Ctx& context) override;
};

class WhileStatement : public ParserBase {
  public:
    explicit WhileStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens,
                                      hzcc::syntax::Ctx& context) override;
};

class Statement : public ParserBase {
  public:
    explicit Statement() noexcept;

    StatusOr<ast::StmtPtr> parse_impl(TokenList& tokens,
                                      hzcc::syntax::Ctx& context) override;

  protected:
    // TODO should be all static function
    static StatusOr<ast::ExprPtr> ParseCommaExpr(Ctx& context,
                                                 TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseAssignExpr(Ctx& context,
                                                  TokenList& tokens);

    // TODO: add syntax for conditional expression
    static StatusOr<ast::ExprPtr> ParseConditionalExpr(Ctx& context,
                                                       TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseLogicalOrExpr(Ctx& context,
                                                     TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseLogicalAndExpr(Ctx& context,
                                                      TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseBitwiseOrExpr(Ctx& context,
                                                     TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseBitwiseXorExpr(Ctx& context,
                                                      TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseBitwiseAndExpr(Ctx& context,
                                                      TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseEqualityExpr(Ctx& context,
                                                    TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseRelationalExpr(Ctx& context,
                                                      TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseShiftExpr(Ctx& context,
                                                 TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseAdditiveExpr(Ctx& context,
                                                    TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseMultiplicativeExpr(Ctx& context,
                                                          TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseUnaryExpr(Ctx& context,
                                                 TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParseAccessExpr(Ctx& context,
                                                  TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParsePostfixExpr(Ctx& context,
                                                   TokenList& tokens);
};

}  // namespace hzcc::syntax::parser
#endif  // MYCC_SYNTAX_HANDLER_BASE_HANDLER_H