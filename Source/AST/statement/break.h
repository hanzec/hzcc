//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_BREAK_H_
#define MYCC_SOURCE_AST_STATEMENT_BREAK_H_
namespace Hzcc::AST {
class BreakStatement : public ASTNode {
    Status visit(ASTVisitor& visitor) override;

  public:
    BreakStatement(std::pair<int, int> pair_1);

  protected:
    [[nodiscard]] const char* GetNodeName() const override;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_BREAK_H_
