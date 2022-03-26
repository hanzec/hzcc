#include <string_view>

#ifndef MYCC_AST_TYPE_H
#define MYCC_AST_TYPE_H

namespace Mycc::AST {
class Type {
  public:
    explicit Type(const std::string& name) : _name(name) {}

    [[nodiscard]] virtual bool IsArray() const { return false; }

    [[nodiscard]] virtual bool IsFuncPtr() const { return false; }
  private:
    std::string_view _name;
};

}  // namespace Mycc::AST

#endif  // MYCC_AST_TYPE_H
