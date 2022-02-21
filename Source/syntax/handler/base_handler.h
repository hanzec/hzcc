#include <memory>
#include <vector>
#include "AST/ast.h"

#ifndef MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
#define MYCC_SYNTAX_HANDLER_BASE_HANDLER_H
namespace Mycc {
namespace Lexical {
class LexicalToken;
} // namespace Lexical
namespace Syntax {
class HandlerBase {
 public:
  HandlerBase();
  virtual ~HandlerBase();
  virtual std::unique_ptr<AST::ExprNode> handle(
      std::vector<Lexical::LexicalToken>& token) = 0;
};
}  // namespace Syntax
}  // namespace Mycc
#endif  // MYCC_SYNTAX_HANDLER_BASE_HANDLER_H