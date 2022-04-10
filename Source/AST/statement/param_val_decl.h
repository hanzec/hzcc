//
// Created by chen_ on 2022/3/29.
//
#include <memory>

#include "AST/type/Type.h"
#include "value_decl.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_PARAM_VAL_DECL_H_
#define MYCC_SOURCE_AST_STATEMENT_PARAM_VAL_DECL_H_
namespace Mycc::AST {
class ParamVarDecl : public VarDecl {
  public:
    ParamVarDecl(const Lexical::Token& name, const std::shared_ptr<Type>& type,
                 std::list<Lexical::Token>& list)
        : VarDecl(type, list, name){};

    void visit(ASTVisitor& visitor) override;


  protected:
    [[nodiscard]] std::string GetNodeName() const override;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_PARAM_VAL_DECL_H_
