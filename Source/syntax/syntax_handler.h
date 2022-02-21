#include "utils/auto_factory.h"
#include "lexical/LexicalToken.h"

#ifndef MYCC_SYNTAX_SYNTAX_HANDLER_H
#define MYCC_SYNTAX_SYNTAX_HANDLER_H
namespace Mycc::Syntax {
class HandlerBase;

class HandlerFactory: public Utils::Factory<HandlerBase, Lexical::LexicalToken::Type> {
 public:
  HandlerFactory();
  ~HandlerFactory();
};



}  // namespace Mycc::Syntax
#endif