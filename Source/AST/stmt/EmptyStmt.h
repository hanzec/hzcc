//
// Created by chen_ on 2022/3/27.
//
#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
#define MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
namespace Hzcc::AST {
class EmptyStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of EmptyStmt
     */
    EmptyStmt();

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get Name of the node
     * @return will always return "EmptyStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

    [[nodiscard]] bool IsEmptyStmt() const override;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;
};

};      // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_EMPTY_H_
