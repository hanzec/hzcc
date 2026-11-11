#ifndef MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#define MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#include <utility>

#include "parser/parser.h"
#include "utils/status/statusor.h"
namespace hzcc::syntax::parser {
/**
 * @brief A parsed declarator type paired with its identifier token.
 */
using VarPack = std::pair<ast::QualTypePtr, Token>;

/**
 * @brief Common parser dispatch interface and shared loop-context flag.
 * @note Parsing consumes the input in place and does not roll back on error.
 * @note Loop state is a shared boolean rather than a nesting stack; parser instances are not reentrant.
 */
class ParserBase {
  public:
    using SyntaxCtx = std::shared_ptr<hzcc::ast::CompilationUnit>;
    /**
     * @brief Store the AST type identifier and diagnostic name for a parser.
     * @param id Hash identifying the produced AST node category.
     * @param name Human-readable name copied into the parser.
     */
    ParserBase(unsigned long id, std::string_view name) noexcept
        : _id(id), _astNodeName(name) {}

    /**
     * @brief Destroy a parser through its common interface.
     */
    virtual ~ParserBase() = default;

    /**
     * @brief Dispatch to the concrete parser implementation.
     * @param ctx Non-null compilation unit providing symbol and scope state.
     * @param token Input token sequence, consumed as the construct is recognized.
     * @return Owned AST node or the first parsing error.
     * @pre Input contains the lookahead required by the concrete grammar rule.
     */
    StatusOr<ast::StmtPtr> parse(SyntaxCtx ctx, TokenList& token) {
        return parse_impl(std::move(ctx), token);
    };

    /**
     * @brief Retrieve the AST category hash associated with this parser.
     * @return Identifier supplied when the parser was constructed.
     */
    [[nodiscard]] size_t id() const { return _id; }

    /**
     * @brief Retrieve the parser diagnostic name.
     * @return View into the name owned by this parser; valid for its lifetime.
     */
    [[nodiscard]] std::string_view name() const { return _astNodeName; }

  protected:
    /**
     * @brief Parse the grammar construct implemented by a concrete parser.
     * @param ctx Non-null compilation unit receiving semantic context updates.
     * @param tokens Token sequence advanced during parsing.
     * @return Owned statement node or a parsing error.
     */
    virtual StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                              TokenList& tokens) = 0;

    /**
     * @brief Inspect the shared loop-context flag.
     * @return the currently stored loop flag.
     */
    static bool WithinLoop() { return _inside_loop; }
    /**
     * @brief Clear the shared loop-context flag.
     * @return false after clearing the flag.
     */
    static bool ExitLoop() { return _inside_loop = false; }
    /**
     * @brief Set the shared loop-context flag.
     * @return true after setting the flag.
     */
    static bool EnterLoop() { return _inside_loop = true; }

  private:
    const size_t _id = -1;
    const std::string _astNodeName;
    inline static bool _inside_loop = false;
};

/**
 * @brief Recursive-descent parser for declarations.
 */
class DeclStatement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit DeclStatement() noexcept;
    /**
     * @brief Parse one declaration from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;

  protected:
    /**
     * @brief Parse comma-separated variable declarators and optional initializers.
     * @param tokens Input beginning at a declarator identifier.
     * @param context Non-null compilation unit used for type and expression parsing.
     * @param base_type Shared base type for subsequent declarators.
     * @param curr_type Fully qualified type of the first declarator.
     * @return Declaration statement owning the variables, or a parsing error.
     */
    static StatusOr<ast::StmtPtr> parse_var(TokenList& tokens,
                                            SyntaxCtx& context,
                                            ast::QualTypePtr base_type,
                                            ast::QualTypePtr curr_type);

    /**
     * @brief Parse a function prototype or definition and update its declaration context.
     * @param tokens Input beginning at the function identifier.
     * @param context Non-null compilation unit used for redeclaration and parameter checks.
     * @param return_type Qualified return type already parsed before the identifier.
     * @return Function declaration node, or a signature/body parsing error.
     */
    static StatusOr<ast::StmtPtr> parse_func(TokenList& tokens,
                                             SyntaxCtx& context,
                                             ast::QualTypePtr return_type);

    /**
     * @brief Parse a struct declaration and its field list.
     * @param context Non-null compilation unit receiving the named record type.
     * @param tokens Input beginning with the struct keyword.
     * @return Record declaration node, or the first field/declaration error.
     */
    static StatusOr<ast::RecordDEclPtr> parse_struct(SyntaxCtx& context,
                                                     TokenList& tokens);

  private:
    /**
     * @brief Parse pointer and array declarator suffixes around an identifier.
     * @param tokens Token sequence beginning at pointer markers or an identifier.
     * @param context Non-null compilation unit used for array-size expression parsing.
     * @param base_type Base type used to build the declarator type.
     * @return Pair of completed type and identifier token, or a declarator error.
     */
    static StatusOr<VarPack> parse_type(TokenList& tokens,             // NOLINT
                                        SyntaxCtx& context,            // NOLINT
                                        ast::QualTypePtr& base_type);  // NOLINT

    bool _is_fist_declare = true;
    uint_fast32_t attribute_size_ = 0;
};

/**
 * @brief Recursive-descent parser for brace-delimited statement blocks.
 */
class CompoundStatement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit CompoundStatement() noexcept;
    /**
     * @brief Parse one compound statement from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

/**
 * @brief Recursive-descent parser for do/while loops.
 */
class DoStatement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit DoStatement() noexcept;
    /**
     * @brief Parse one do/while loop from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

/**
 * @brief Recursive-descent parser for for loops.
 */
class ForStatement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit ForStatement() noexcept;
    /**
     * @brief Parse one for loop from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

/**
 * @brief Recursive-descent parser for if statements and their alternative branches.
 */
class IfStatement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit IfStatement() noexcept;
    /**
     * @brief Parse one if statement from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

/**
 * @brief Recursive-descent parser for while loops.
 */
class WhileStatement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit WhileStatement() noexcept;
    /**
     * @brief Parse one while loop from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;
};

/**
 * @brief Recursive-descent parser for statements and precedence-ordered expressions.
 */
class Statement : public ParserBase {
  public:
    /**
     * @brief Initialize the parser identity for its AST node category.
     */
    explicit Statement() noexcept;

    /**
     * @brief Parse one statement or expression from the front of the token sequence.
     * @param ctx Non-null compilation unit supplying types, declarations, and scope state.
     * @param tokens Input tokens consumed while constructing the AST.
     * @return Owned AST node on success, or the first parsing error.
     * @pre The token sequence contains the expected construct and its required lookahead.
     */
    StatusOr<ast::StmtPtr> parse_impl(SyntaxCtx ctx,
                                      TokenList& tokens) override;

  protected:
    // TODO should be all static function
    /**
     * @brief Parse comma expressions, recursively joining assignment expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param context Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseCommaExpr(TokenList& tokens,
                                                 const SyntaxCtx& context);

    /**
     * @brief Parse simple and compound assignment expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseAssignExpr(TokenList& tokens,
                                                  const SyntaxCtx& ctx);

    // TODO: add syntax for conditional expression
    /**
     * @brief Parse conditional expressions with question-mark and colon branches.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseConditionalExpr(TokenList& tokens,
                                                       const SyntaxCtx& ctx);

    /**
     * @brief Parse logical-or expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseLogicalOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx);

    /**
     * @brief Parse logical-and expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseLogicalAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    /**
     * @brief Parse bitwise-or expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseBitwiseOrExpr(TokenList& tokens,
                                                     const SyntaxCtx& ctx);

    /**
     * @brief Parse bitwise-exclusive-or expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseBitwiseXorExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    /**
     * @brief Parse bitwise-and expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseBitwiseAndExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    /**
     * @brief Parse equality and inequality comparisons.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseEqualityExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx);

    /**
     * @brief Parse ordered relational comparisons.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseRelationalExpr(TokenList& tokens,
                                                      const SyntaxCtx& ctx);

    /**
     * @brief Parse left and right shift expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseShiftExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx);

    /**
     * @brief Parse addition and subtraction expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseAdditiveExpr(TokenList& tokens,
                                                    const SyntaxCtx& ctx);

    /**
     * @brief Parse multiplication, division, and remainder expressions.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseMultiplicativeExpr(TokenList& tokens,
                                                          const SyntaxCtx& ctx);

    /**
     * @brief Parse prefix unary operations and their operands.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseUnaryExpr(TokenList& tokens,
                                                 const SyntaxCtx& ctx);
    /**
     * @brief Parse explicit type casts or the next unary expression.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseCastExpr(TokenList& tokens,
                                                const SyntaxCtx& ctx);

    /**
     * @brief Parse primary operands and supported access constructs.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParseAccessExpr(const SyntaxCtx& ctx,
                                                  TokenList& tokens);

    /**
     * @brief Parse postfix operations, calls, subscripts, and member access.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned expression node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::ExprPtr> ParsePostfixExpr(TokenList& tokens,
                                                   const SyntaxCtx& ctx);

    /**
     * @brief Parse a primitive or named type followed by pointer markers.
     * @param tokens Input sequence consumed through the recognized expression or type.
     * @param ctx Non-null compilation unit used to resolve names and types.
     * @return Owned type-proxy node, or the first parsing error.
     * @pre Input provides all lookahead required by this grammar rule.
     */
    static StatusOr<ast::TypeProxyExprPtr> ParseTypeName(TokenList& tokens,
                                                         const SyntaxCtx& ctx);
};

}  // namespace hzcc::syntax::parser
#endif  // MYCC_SYNTAX_HANDLER_BASE_HANDLER_H