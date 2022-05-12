//
// Created by chen_ on 2022/3/23.
//

#ifndef MYCC_PARSER_H
#define MYCC_PARSER_H

#include <cassert>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "AST/ASTNode.h"
#include "AST/statement/compound.h"
#include "AST/statement/do.h"
#include "AST/statement/for.h"
#include "AST/statement/function_decl.h"
#include "AST/statement/if.h"
#include "AST/statement/struct.h"
#include "AST/statement/value_decl.h"
#include "SyntaxContext.h"
#include "parser/base_parser.h"
#include "parser/statement_parser.h"
#include "syntax/parser/function_parser.h"
#include "syntax/parser/statements/compound_parser.h"
#include "syntax/parser/statements/do_parser.h"
#include "syntax/parser/statements/for_parser.h"
#include "syntax/parser/statements/if_parser.h"
#include "syntax/parser/statements/struct_parser.h"
#include "syntax/parser/statements/value_decl_parser.h"
#include "syntax/parser/statements/while_parser.h"
#include "utils/type_name_utils.h"
namespace Hzcc::Syntax {
class ParserFactory {
  public:
    template <class name>
    static std::unique_ptr<name> ParseAST(TokenList& tokens,
                                          SyntaxContext& context) {
        return std::unique_ptr<name>(
            static_cast<name*>(GetParser<name>(TypeNameUtil::hash<name>())
                                   ->parse(tokens, context)
                                   .release()));
    }

  protected:
    template <class name>
    static std::shared_ptr<Parser::ParserBase> GetParser(size_t parser) {
        static std::shared_ptr<Parser::ParserBase> parser_ = _parserMap[parser];

        DLOG_ASSERT(parser_ != nullptr)
            << "Parser not found: " << TypeNameUtil::name_pretty<name>();
        return parser_;
    }

  private:
    inline static std::unordered_map<size_t,
                                     std::shared_ptr<Parser::ParserBase>>
        _parserMap = {{TypeNameUtil::hash<AST::ASTNode>(),
                       std::make_shared<Parser::Statement>()},
                      {TypeNameUtil::hash<AST::FunctionDeclNode>(),
                       std::make_shared<Parser::Function>()},
                      {TypeNameUtil::hash<AST::CompoundStmt>(),
                       std::make_shared<Parser::CompoundStatement>()},
                      {TypeNameUtil::hash<AST::DoStatement>(),
                       std::make_shared<Parser::DoStatement>()},
                      {TypeNameUtil::hash<AST::ForStatement>(),
                       std::make_shared<Parser::ForStatement>()},
                      {TypeNameUtil::hash<AST::IfStatement>(),
                       std::make_shared<Parser::IfStatement>()},
                      {TypeNameUtil::hash<AST::WhileStatement>(),
                       std::make_shared<Parser::WhileStatement>()},
                      {TypeNameUtil::hash<AST::VarDecl>(),
                       std::make_shared<Parser::ValueDeclare>()},
                      {TypeNameUtil::hash<AST::StructDeclareNode>(),
                       std::make_shared<Parser::StructDeclare>()}};
};

#ifndef NDEBUG
class ParserFactoryReporter {
  public:
    ParserFactoryReporter(std::string Caller, std::string File, int Line)
        : _caller(std::move(Caller)), _file(std::move(File)), _line(Line) {}

    static std::shared_ptr<ParserFactoryReporter> GetReporter(
        const std::string& caller, const std::string& file, int line) {
        return std::make_shared<ParserFactoryReporter>(caller, file, line);
    }

    template <class name>
    std::unique_ptr<name> ParseAST(TokenList& tokens, SyntaxContext& context) {
        DVLOG(SYNTAX_LOG_LEVEL)
            << "\nRequest Parser From: [" << _caller << "] " << _file << ":"
            << _line << "\n"
            << "\tParseAST: " << TypeNameUtil::name_pretty<name>() << "\n"
            << "\tToken: " << MYCC_PRETTY_PRINT_TOKEN(tokens.peek()) << "\n"
            << "\tattributes: []"
            << "\n"
            << "\tUsingParser: " << std::hex << TypeNameUtil::hash<name>();
        return ParserFactory::ParseAST<name>(tokens, context);
    }

  private:
    int _line;
    std::string _file;
    std::string _caller;
};

// remove the symbol for the function, then define a new version that instead
// creates a stack temporary instance of ClassNameDecorator.
// FunctionName is then replaced with a version that takes the caller
// information and uses Method Chaining to allow expr() to be invoked with
// the original parameters.
#undef ParserFactory
#define ParserFactory ParserFactoryReporter
#undef ParseAST
#define ParseAST GetReporter(__FUNCTION__, __FILE__, __LINE__)->ParseAST
#endif

}  // namespace Hzcc::Syntax

#endif  // MYCC_PARSER_H
