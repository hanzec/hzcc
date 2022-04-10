//
// Created by chen_ on 2022/3/27.
//
#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
#define MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
namespace Mycc::AST {
class EmptyStatement : public ASTNode {
    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    void visit(ASTVisitor& visitor) override;


  protected:
    [[nodiscard]] std::string GetNodeName() const override;
};

};      // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
