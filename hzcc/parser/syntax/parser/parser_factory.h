//
// Created by chen_ on 2022/3/23.
//

#ifndef MYCC_PARSER_H
#define MYCC_PARSER_H
#include <memory>

#include "parser/syntax/parser/syntax_parser.h"
#include "utils/logging.h"
#include "utils/status/statusor.h"
#include "utils/type_name_utils.h"
namespace hzcc::syntax {
class Parser {
  public:
    using SyntaxCtx = std::shared_ptr<ast::CompilationUnit>;
    template <class RetClass,                                    // NOLINT
              typename RetClassPtr = std::unique_ptr<RetClass>>  // NOLINT
    static StatusOr<RetClassPtr> Parse(SyntaxCtx ctx, TokenList& tokens) {
        auto ret = GetParser<RetClass>()->parse(ctx, tokens);

        if (ret.ok()) {
            return std::unique_ptr<RetClass>(
                static_cast<RetClass*>(ret.value().release()));
        } else {
            DLOG(ERROR) << "[" << TypeNameUtil::name_pretty<RetClass>()
                        << "] failed: " << ret.status().ToString();
            return ret.status();
        }
    }

  protected:
    template <class RequestParser>
    static std::shared_ptr<parser::ParserBase> GetParser() {
        static std::shared_ptr<parser::ParserBase> parser_ =
            _parserMap[TypeNameUtil::hash<RequestParser>()];

        DLOG_ASSERT(parser_ != nullptr)
            << "parser not found: "
            << TypeNameUtil::name_pretty<RequestParser>();
        return parser_;
    }

  private:
    inline static std::unordered_map<size_t,
                                     std::shared_ptr<parser::ParserBase>>
        _parserMap = {{TypeNameUtil::hash<ast::Stmt>(),
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
class ParserFactoryReporter {
  public:
    using SyntaxCtx = std::shared_ptr<ast::CompilationUnit>;
    ParserFactoryReporter(std::string Caller, std::string File, int Line)
        : _caller(std::move(Caller)), _file(std::move(File)), _line(Line) {}

    static std::shared_ptr<ParserFactoryReporter> GetReporter(
        const std::string& caller, const std::string& file, int line) {
        return std::make_shared<ParserFactoryReporter>(caller, file, line);
    }

    template <class name>
    StatusOr<std::unique_ptr<name>> Parse(SyntaxCtx ctx,
                                          TokenList& tokens) const {
        DVLOG(SYNTAX_LOG_LEVEL)
            << "\nRequest parser From: [" << _caller << "] " << _file << ":"
            << _line << "\n"
            << "\tAST: " << TypeNameUtil::name_pretty<name>() << "\n"
            << "\tToken: " << HZCC_PRETTY_PRINT_TOKEN(tokens.peek()) << "\n"
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
