//
// Created by chen_ on 2022/3/28.
//
#include <string>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_DECL_H_
#define MYCC_SOURCE_AST_STATEMENT_DECL_H_
namespace Mycc {
namespace Lexical {
class Token;
}
namespace AST {
class DeclNode : public ASTNode {
  public:
    explicit DeclNode(const Lexical::Token& decl_name);

    void visit(ASTVisitor& visitor) override;

    DeclNode(const Lexical::Token& decl_loc, std::string decl_name);

    [[nodiscard]] bool IsDeclNode() const override { return true; }

    [[nodiscard]] std::string GetName() const { return _decl_name; }

    [[nodiscard]] virtual bool IsFuncDecl() const { return false; }

  private:
    std::string _decl_name;
};
}  // namespace AST
}  // namespace Mycc
#endif  // MYCC_SOURCE_AST_STATEMENT_DECL_H_
