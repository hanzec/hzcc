//
// Created by chen_ on 2022/3/25.
//
#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_CONTINUE_H_
#define MYCC_SOURCE_AST_STATEMENT_CONTINUE_H_
namespace Mycc::AST {
class ContinueStatement : public ASTNode {
    void visit(ASTVisitor& visitor) override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_CONTINUE_H_
