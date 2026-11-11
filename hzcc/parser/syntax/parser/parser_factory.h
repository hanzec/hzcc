//
// Created by chen_ on 2022/3/23.
//

#ifndef MYCC_PARSER_H
#define MYCC_PARSER_H
#include <memory>
#include <stdexcept>
#include <string>

#include "parser/syntax/parser/syntax_parser.h"
#include "utils/logging.h"
#include "utils/status/statusor.h"
#include "utils/type_name_utils.h"
namespace hzcc::syntax {
/**
 * @brief Select a registered parser by the AST node type requested by the caller.
 * @note Parser instances and their loop flag are shared; this factory is not reentrant.
 */
class Parser {
  public:
    using SyntaxCtx = std::shared_ptr<ast::CompilationUnit>;
    /**
     * @brief Parse a construct using the parser registered for the requested AST class.
     * @tparam RetClass AST class whose parser and result type must match.
     * @tparam RetClassPtr Owning result pointer type, normally std::unique_ptr<RetClass>.
     * @param ctx Non-null compilation unit used during parsing.
     * @param tokens Input sequence consumed by the selected parser.
     * @return Owned AST node, an unimplemented error for an unregistered AST category,
     * or the parser/validation error status. Truncated lookahead becomes a syntax error.
     * @note Consumed input is not rolled back when a parser returns an error.
     */
    template <class RetClass,                                    // NOLINT
              typename RetClassPtr = std::unique_ptr<RetClass>>  // NOLINT
    static StatusOr<RetClassPtr> Parse(SyntaxCtx ctx, TokenList& tokens) {
        if (!ctx) {
            return InvalidArgumentError("A compilation unit is required for parsing");
        }
        auto parser = GetParser<RetClass>();
        if (!parser) {
            return UnimplementedError(std::string("No parser registered for ") +
                                      std::string(TypeNameUtil::name_pretty<RetClass>()));
        }
        if (tokens.empty()) {
            return Status(StatusCode::kSyntaxStageErr, "Unexpected end of token sequence");
        }
        try {
            auto ret = parser->parse(std::move(ctx), tokens);
            if (!ret.ok()) {
                return ret.status();
            }
            auto* node = dynamic_cast<RetClass*>(ret.value().get());
            if (!node) {
                return InternalError("Parser returned an incompatible AST node type");
            }
            ret.value().release();
            return std::unique_ptr<RetClass>(node);
        } catch (const std::out_of_range&) {
            // CachedList lookahead reports truncated grammar input by throwing.
            return Status(StatusCode::kSyntaxStageErr, "Unexpected end of token sequence");
        }
    }

  protected:
    /**
     * @brief Retrieve the parser registered for an AST category without inserting entries.
     * @tparam RequestParser AST class used to compute the registration key.
     * @return Shared parser instance, or nullptr when no registration exists.
     */
    template <class RequestParser>
    static std::shared_ptr<parser::ParserBase> GetParser() {
        auto found = _parserMap.find(TypeNameUtil::hash<RequestParser>());
        return found == _parserMap.end() ? nullptr : found->second;
    }

  private:
    inline static std::unordered_map<size_t,
                                     std::shared_ptr<parser::ParserBase>>
        _parserMap = {{TypeNameUtil::hash<ast::IDeclStmt>(),
                       std::make_shared<parser::DeclStatement>()},
                      {TypeNameUtil::hash<ast::Stmt>(),
                       std::make_shared<parser::Statement>()},
                      {TypeNameUtil::hash<ast::CompoundStmt>(),
                       std::make_shared<parser::CompoundStatement>()},
                      {TypeNameUtil::hash<ast::DoStmt>(),
                       std::make_shared<parser::DoStatement>()},
                      {TypeNameUtil::hash<ast::ForStmt>(),
                       std::make_shared<parser::ForStatement>()},
                      {TypeNameUtil::hash<ast::IfStmt>(),
                       std::make_shared<parser::IfStatement>()},
                      {TypeNameUtil::hash<ast::WhileStmt>(),
                       std::make_shared<parser::WhileStatement>()}};
};

#ifndef NDEBUG
/**
 * @brief Debug-only adapter that logs the caller and token count before parser dispatch.
 */
class ParserFactoryReporter {
  public:
    using SyntaxCtx = std::shared_ptr<ast::CompilationUnit>;
    /**
     * @brief Capture a parser call site for diagnostic tracing.
     * @param Caller Calling function name.
     * @param File Source filename containing the call.
     * @param Line Source line of the call.
     */
    ParserFactoryReporter(std::string Caller, std::string File, int Line)
        : _caller(std::move(Caller)), _file(std::move(File)), _line(Line) {}

    /**
     * @brief Allocate a reporter for one parser call site.
     * @param caller Calling function name.
     * @param file Source filename containing the call.
     * @param line Source line of the call.
     * @return Shared ownership of a new call-site reporter.
     */
    static std::shared_ptr<ParserFactoryReporter> GetReporter(
        const std::string& caller, const std::string& file, int line) {
        return std::make_shared<ParserFactoryReporter>(caller, file, line);
    }

    /**
     * @brief Log parser dispatch details, then forward to Parser::Parse.
     * @tparam name Requested AST node class.
     * @param ctx Non-null compilation unit used by the parser.
     * @param tokens Input sequence; its remaining size is included in the trace.
     * @return Owned AST node or the forwarded parser error.
     */
    template <class name>
    StatusOr<std::unique_ptr<name>> Parse(SyntaxCtx ctx,
                                          TokenList& tokens) const {
        DVLOG(SYNTAX_LOG)
            << "\nRequest parser From: [" << _caller << "] " << _file << ":"
            << _line << "\n"
            << "\tAST: " << TypeNameUtil::name_pretty<name>() << "\n"
            << "\tRemaining tokens: " << tokens.size() << "\n"
            << "\tUsingParser: " << std::hex << TypeNameUtil::hash<name>();
        return Parser::Parse<name>(ctx, tokens);
    }

  private:
    int _line;
    std::string _file;
    std::string _caller;
};

// remove the symbol for the function, then define a new version that instead
// creates a stack temporary instance of ClassNameDecorator.
// FunctionName is then replaced with a version that takes the caller
// information and uses Method Chaining to allow cast() to be invoked with
// the original parameters.
#undef Parser
#define Parser ParserFactoryReporter
#undef Parse
#define Parse GetReporter(__FUNCTION__, __FILE__, __LINE__)->Parse
#endif

}  // namespace hzcc::syntax

#endif  // MYCC_PARSER_H
