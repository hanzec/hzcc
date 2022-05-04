//
// Created by chen_ on 2022/3/25.
//
#include <memory>
#include <string>
#include <utility>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ACCESS_H_
#define MYCC_SOURCE_AST_OPERATOR_ACCESS_H_
namespace Hzcc::AST {
class AccessExpr : public ASTNode {
  public:
    AccessExpr(bool isPtr, const Lexical::Token& field,
               std::unique_ptr<ASTNode> expr);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsAssignable() const override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] const char* GetNodeName() const override;

  private:
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<ASTNode> _varaible;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ACCESS_H_
