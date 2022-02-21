#include "base_handler.h"
#include "syntax/syntax_handler.h"

namespace Mycc::Syntax::Handler {
class TypedefExp : public HandlerBase,
                   private Utils::AutoRegProduct<TypedefExp, HandlerFactory> {
  REG_PRODUCT_PRIORITY(TypedefExp,Lexical::LexicalToken::Type::kTypedef,HandlerFactory, -1);
  
};
}  // namespace Mycc::Syntax::Handler