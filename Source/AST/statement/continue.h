//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_CONTINUE_H_
#define MYCC_SOURCE_AST_STATEMENT_CONTINUE_H_
namespace Hzcc::AST {
class ContinueStatement : public ASTNode {
    Status visit(ASTVisitor& visitor) override;

  public:
    explicit ContinueStatement(std::pair<int, int> pair_1);

  protected:
    [[nodiscard]] const char* NodeName() const override;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_CONTINUE_H_
