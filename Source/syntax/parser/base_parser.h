#include <list>
#include <memory>
#include <vector>

#include "options.h"
#include "syntax/utils/token_list_utils.h"
#include "utils/Status.h"
#include "utils/message_utils.h"
#ifndef MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#define MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
namespace Mycc {
namespace AST {
class ASTNode;
class ASTContext;
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

    std::unique_ptr<AST::ASTNode> parse(AST::ASTContext& context,
                                        TokenList& token) {
        return parse_impl(context, token);
    };

    std::unique_ptr<AST::ASTNode> parse(AST::ASTContext& context,
                                        TokenList& token,
                                        TokenList& attributes) {
        // attributes should not have any duplicate token or attribute is empty
        if (attributes.empty()) {
            return parse_impl(context, token);
        } else if (!checkDuplicateToken(attributes)) {
            return parse_impl(context, token, attributes);
        }
        return nullptr;
    };

    [[nodiscard]] size_t ParserID() const { return _id; }

    [[nodiscard]] std::string_view GetASTNodeName() const {
        return _astNodeName;
    }

  protected:
    virtual std::unique_ptr<AST::ASTNode> parse_impl(AST::ASTContext& context,
                                                     TokenList& tokens) {
        return parse_impl(context, tokens, kEmptyTokenList);
    };

    virtual std::unique_ptr<AST::ASTNode> parse_impl(  // NOLINT
        AST::ASTContext& context,                      // NOLINT
        TokenList& token,                              // NOLINT
        TokenList& attributes) {                       // NOLINT
        MYCC_PrintFirstTokenError_ReturnNull(
            attributes, error_token.Value() + " does not allow to apply here");
    };

    static bool checkDuplicateToken(const TokenList& attributes) {
        auto dup_attrs = TokenListUtils::checkDuplicate(attributes);
        if (dup_attrs.has_value()) {
            MYCC_PrintTokenError(
                dup_attrs.value(),
                "Expect an identifier after the type definition, but got " +
                    dup_attrs.value().Value()) return true;
        }
        return false;
    }

  private:
    const size_t _id = -1;
    const std::string_view _astNodeName;
    inline static TokenList kEmptyTokenList;
};

}  // namespace Syntax::Parser
}  // namespace Mycc
#endif  // MYCC_SYNTAX_HANDLER_BASE_HANDLER_H