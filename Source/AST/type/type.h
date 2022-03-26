#include <string_view>

#ifndef MYCC_AST_TYPE_H
#define MYCC_AST_TYPE_H

namespace Mycc::AST{
class Type{
  private:
    std::string_view _name;
};

} // namespace Mycc::AST

#endif  // MYCC_AST_TYPE_H
