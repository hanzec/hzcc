//
// Created by chen_ on 2022/3/23.
//

#ifndef MYCC_PARSER_H
#define MYCC_PARSER_H

#include <memory>
#include <string_view>
#include <unordered_map>

#include "AST/ast_node.h"
#include "AST/function_node.h"
#include "AST/statement/block_parser.h"
#include "AST/statement/do_parser.h"
#include "AST/statement/for_parser.h"
#include "AST/statement/if_parser.h"
#include "parser/base_parser.h"
#include "parser/statement_parser.h"
#include "syntax/parser/function_parser.h"
#include "syntax/parser/statements/block_parser.h"
#include "syntax/parser/statements/do_parser.h"
#include "syntax/parser/statements/for_parser.h"
#include "syntax/parser/statements/if_parser.h"
#include "syntax/parser/statements/while_parser.h"
#include "utils/debug_utils.h"
#include "utils/type_name_utils.h"
namespace Mycc::Syntax {
class ParserFactory {
  public:
    template <class name>
    static std::unique_ptr<AST::ASTNode> ParseAST(
        AST::ASTContext& context,  // NOLINT
        std::list<Lexical::Token>& tokens) {
        return GetParser<name>(TypeNameUtil::hash<name>())
            ->parse(context, tokens);
    }

    template <class name>
    static std::unique_ptr<AST::ASTNode> ParseAST(
        AST::ASTContext& context,           // NOLINT
        std::list<Lexical::Token>& tokens,  // NOLINT
        const std::list<Lexical::Token>& attributes) {
#ifndef NDEBUG
        DLOG(INFO) << "\nParseAST: " << TypeNameUtil::name_pretty<name>()
                   << "\n"
                   << "\tToken: \033[0;33m"
                   << ((int)tokens.front().Type() <= 0xFF
                           ? "[Symbol"
                           : "[" + Lexical::SymbolUtils::TokenTypeToString(
                                       tokens.front().Type()))
                   << "]:@" << tokens.front().Value() << "@\033[0m"
                   << "\n"
                   << "\tattributes: " << Debug::PrintAttributeList(attributes)
                   << "\n"
                   << "\tUsingParser: " << std::hex
                   << GetParser<name>(TypeNameUtil::hash<name>())->ParserID();
#endif
        return GetParser<name>(TypeNameUtil::hash<name>())
            ->parse(context, tokens, attributes);
    }

  protected:
    template <class name>
    static std::shared_ptr<Parser::ParserBase> GetParser(size_t parser) {
        static std::shared_ptr<Parser::ParserBase> parser_ = _parserMap[parser];
        return parser_;
    }

  private:
    inline static std::unordered_map<size_t,
                                     std::shared_ptr<Parser::ParserBase>>
        _parserMap = {{TypeNameUtil::hash<AST::ASTNode>(),
                       std::make_shared<Parser::Statement>()},
                      {TypeNameUtil::hash<AST::FunctionNode>(),
                       std::make_shared<Parser::Function>()},
                      {TypeNameUtil::hash<AST::BlockStatement>(),
                       std::make_shared<Parser::BlockStatement>()},
                      {TypeNameUtil::hash<AST::DoStatement>(),
                       std::make_shared<Parser::DoStatement>()},
                      {TypeNameUtil::hash<AST::ForStatement>(),
                       std::make_shared<Parser::ForStatement>()},
                      {TypeNameUtil::hash<AST::IfStatement>(),
                       std::make_shared<Parser::IfStatement>()},
                      {TypeNameUtil::hash<AST::WhileStatement>(),
                       std::make_shared<Parser::WhileStatement>()}};
};
}  // namespace Mycc::Syntax

#endif  // MYCC_PARSER_H
