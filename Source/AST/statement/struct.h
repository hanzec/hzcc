//
// Created by chen_ on 2022/3/27.
//
#include <string>
#include <utility>

#include "AST/ASTNode.h"
#include "AST/type/StructType.h"
#include "AST/type/Type.h"
#include "decl.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_STRUCT_DECL_H_
#define MYCC_SOURCE_AST_STATEMENT_STRUCT_DECL_H_
namespace Mycc::AST {
class StructDeclareNode : public DeclNode {
  public:
    explicit StructDeclareNode(const std::string& name,
                               const Lexical::Token& token,

                               std::shared_ptr<StructType> type);

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool IsStructDecl() const override { return true; }

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    const std::shared_ptr<Type> _type;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_STRUCT_DECL_H_
