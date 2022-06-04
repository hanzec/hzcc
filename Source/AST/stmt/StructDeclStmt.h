//
// Created by chen_ on 2022/3/27.
//
#include "AST/ASTNode.h"
#include "AST/type/StructType.h"
#include "AST/type/Type.h"
#include "DeclStmt.h"
#ifndef HZCC_AST_STATEMENT_STRUCT_DECLARE_NODE_H
#define HZCC_AST_STATEMENT_STRUCT_DECLARE_NODE_H
namespace Hzcc::AST {
class StructDeclStmt : public DeclStmt {
  public:
    /**
     * @brief Constructor of StructDeclStmt
     * @param name name of the struct
     * @param type type of the struct
     * @param location location of the struct
     */
    StructDeclStmt(const std::string_view& name,          // NOLINT
                   std::shared_ptr<StructType> type,      // NOLINT
                   const std::pair<int, int>& location);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    [[nodiscard]] bool IsStructDecl() const override { return true; }

    /**
     * @brief Get Name of the node
     * @return will always return "StructDeclStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

  private:
    const std::shared_ptr<Type> _type;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_STRUCT_DECLARE_NODE_H
