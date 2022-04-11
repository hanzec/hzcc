//
// Created by chen_ on 2022/3/26.
//
#include <list>
#include <string>
#include <string_view>
#include <utility>

#include "AST/type/Type.h"
#include "decl.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_VALUE_DECL_H_
#define MYCC_SOURCE_AST_STATEMENT_VALUE_DECL_H_

namespace Mycc::AST {
class VarDecl : public DeclNode {
  public:
    explicit VarDecl(std::shared_ptr<Type> type,
                     std::list<Lexical::Token>& attrs,
                     const Lexical::Token& decl_name);

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::shared_ptr<Type> _type;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_VALUE_DECL_H_
