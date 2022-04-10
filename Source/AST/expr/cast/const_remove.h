//
// Created by chen_ on 2022/3/29.
//
#include "cast.h"
#ifndef MYCC_SOURCE_AST_CAST_CONST_REMOVE_H_
#define MYCC_SOURCE_AST_CAST_CONST_REMOVE_H_
namespace Mycc::AST {
class ConstRemoveCast : public CastExpr {
  public:
    explicit ConstRemoveCast(std::unique_ptr<ASTNode> expr,
                             const std::shared_ptr<Type>& type);

    void visit(ASTVisitor& visitor) override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<ASTNode> _varaible;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_CAST_CONST_REMOVE_H_
