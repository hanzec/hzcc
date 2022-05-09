//
// Created by chen_ on 2022/3/27.
//
#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
#define MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
namespace Hzcc::AST {
class EmptyStatement : public ASTNode {
  public:
    explicit EmptyStatement(std::pair<int, int> pair_1);

    bool IsEmptyStmt() const override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    Status visit(ASTVisitor& visitor) override;

  protected:
    [[nodiscard]] const char* NodeName() const override;
};

};      // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
