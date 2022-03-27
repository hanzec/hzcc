#include <string>
#include <utility>

#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_VARIABLES_H
#define MYCC_SOURCE_AST_VARIABLES_H
namespace Mycc::AST {
class DeclRefExpr : public ASTNode {
  public:
    explicit DeclRefExpr(const Lexical::Token& name, std::shared_ptr<Type> type);

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] bool IsAssignable() const override;


  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;
  private:
    std::string _name{};
    std::shared_ptr<Type> _type{};
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_VARIABLES_H