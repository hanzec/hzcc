//
// Created by chen_ on 2022/3/25.
//
#include <cassert>

#include "AST/expr/operator/operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
#define MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
namespace Hzcc::AST {
enum PACKED LogicalType {
    kLogicalType_And = 0,
    kLogicalType_Or = 1,
    kLogicalType_ENUM_SIZE = 2,
};

class LogicalExpr : public OperatorBase {
  public:
    LogicalExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs);

    Status visit(ASTVisitor& visitor) override;

  protected:
    /**
     * @brief Get node name
     * @return node name
     */
    [[nodiscard]] const char* GetNodeName() const override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  private:
    LogicalType _type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
