#ifndef MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#define MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#include <utility>

#include "parser/parser.h"
#include "utils/status/statusor.h"
namespace hzcc::syntax::parser {
using VarPack = std::pair<ast::QualTypePtr, Token>;

class ParserBase {
  public:
    using SyntaxCtx = std::shared_ptr<hzcc::ast::CompilationUnit>;
    ParserBase(unsigned long id, std::string_view name) noexcept
        : _id(id), _astNodeName(name) {}

    virtual ~ParserBase() = default;

    StatusOr<ast::StmtPtr> parse(SyntaxCtx ctx, TokenList& token) {
        return parse_impl(std::move(ctx), token);
    };

    [[nodiscard]] size_t id() const { return _id; }

    [[nodiscard]] std::string_view name() const { return _astNodeName; }

  protected:
    virtual StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                              TokenList& tokens) = 0;

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
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;

  protected:
    static StatusOr<ast::StmtPtr> parse_var(TokenList& tokens,
                                            SyntaxCtx& context,
                                            ast::QualTypePtr base_type,
                                            ast::QualTypePtr curr_type);

    static StatusOr<ast::StmtPtr> parse_func(TokenList& tokens,
                                             SyntaxCtx& context,
                                             ast::QualTypePtr return_type);

    static StatusOr<ast::RecordDEclPtr> parse_struct(SyntaxCtx& context,
                                                     TokenList& tokens);

  private:
    static StatusOr<VarPack> parse_type(TokenList& tokens,             // NOLINT
                                        SyntaxCtx& context,            // NOLINT
                                        ast::QualTypePtr& base_type);  // NOLINT

    bool _is_fist_declare = true;
    uint_fast32_t attribute_size_ = 0;
};

class CompoundStatement : public ParserBase {
  public:
    explicit CompoundStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

class DoStatement : public ParserBase {
  public:
    explicit DoStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

class ForStatement : public ParserBase {
  public:
    explicit ForStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

class IfStatement : public ParserBase {
  public:
    explicit IfStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

class WhileStatement : public ParserBase {
  public:
    explicit WhileStatement() noexcept;
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

class Statement : public ParserBase {
  public:
    explicit Statement() noexcept;

    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;

  protected:
    // TODO should be all static function
    static StatusOr<ast::ExprPtr> ParseCommaExpr(TokenList& tokens,
                                                 const SyntaxCtx& context);

    static StatusOr<ast::ExprPtr> ParseAssignExpr(TokenList& tokens,
                                                  const SyntaxCtx& ctx);

    // TODO: add syntax for conditional expression
    static StatusOr<ast::ExprPtr> ParseConditionalExpr(TokenList& tokens,
                                                       const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseLogicalOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseLogicalAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseBitwiseOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseBitwiseXorExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseBitwiseAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseEqualityExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseRelationalExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseShiftExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseAdditiveExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseMultiplicativeExpr(TokenList& tokens,
                                                          const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseUnaryExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx);
    static StatusOr<ast::ExprPtr> ParseCastExpr(TokenList& tokens,
                                                const SyntaxCtx& ctx);

    static StatusOr<ast::ExprPtr> ParseAccessExpr(const SyntaxCtx& ctx,
                                                  TokenList& tokens);

    static StatusOr<ast::ExprPtr> ParsePostfixExpr(TokenList& tokens,
                                                   const SyntaxCtx& ctx);

    static StatusOr<ast::TypeProxyExprPtr> ParseTypeName(TokenList& tokens,
                                                         const SyntaxCtx& ctx);
};

}  // namespace hzcc::syntax::parser
#endif  // MYCC_SYNTAX_HANDLER_BASE_HANDLER_H