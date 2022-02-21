#include "handler_base.h"
#include "syntax/syntax_handler.h"

namespace Mycc::Syntax::Handler {
  class VarExp : public Utils::AutoRegProduct<HandlerFactory> {
    REG_PRODUCT(Lexical::LexicalToken::Type::kIdentifier, &HandlerBase::handleIdentifier);
  public:
    HandlerFactory();
    ~HandlerFactory();
  };
} // namespace Mycc::Syntax
