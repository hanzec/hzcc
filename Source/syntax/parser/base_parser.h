#include <list>
#include <memory>
#include <vector>

#include "options.h"
#include "syntax/utils/common_utils.h"
#include "utils/Status.h"
#include "utils/message_utils.h"
#ifndef MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#define MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
namespace Hzcc {
namespace AST {
class ASTNode;
class CompilationUnit;
}  // namespace AST
namespace Lexical {
class Token;
}  // namespace Lexical
namespace Syntax::Parser {
class ParserBase {
  public:
    ParserBase(size_t id, std::string_view ASTNodeName) noexcept
        : _id(id), _astNodeName(ASTNodeName) {}
    virtual ~ParserBase() = default;

    std::unique_ptr<AST::ASTNode> parse(TokenList& token,
                                        SyntaxContext& context) {
        return parse_impl(token, context);
    };

    [[nodiscard]] size_t ParserID() const { return _id; }

    [[nodiscard]] std::string_view GetASTNodeName() const {
        return _astNodeName;
    }

  protected:
    virtual std::unique_ptr<AST::ASTNode> parse_impl(
        TokenList& tokens, SyntaxContext& context) = 0;

    static bool WithinLoop() { return _inside_loop; }
    static bool ExitLoop() { return _inside_loop = false; }
    static bool EnterLoop() { return _inside_loop = true; }

  private:
    const size_t _id = -1;
    const std::string_view _astNodeName;
    inline static bool _inside_loop = false;
};

}  // namespace Syntax::Parser
}  // namespace Hzcc
#endif  // MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
