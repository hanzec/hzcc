//
// Created by chen_ on 2022/3/27.
//
#include <string>
#include <utility>

#include "AST/ast_node.h"
#include "AST/type/struct_type.h"
#include "AST/type/type.h"
#include "decl.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_STRUCT_DECL_H_
#define MYCC_SOURCE_AST_STATEMENT_STRUCT_DECL_H_
namespace Mycc::AST {
class StructDeclareNode : public DeclNode {
  public:
    explicit StructDeclareNode(const Lexical::Token& token,
                               std::shared_ptr<StructType> type);

    [[nodiscard]]  bool HasBody() const override{ return true; }

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    const std::string _name;
    const std::shared_ptr<Type> _type;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_STRUCT_DECL_H_
