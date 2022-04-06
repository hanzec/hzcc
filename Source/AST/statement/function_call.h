//
// Created by chen_ on 2022/3/27.
//
#include <list>
#include <memory>
#include <string>
#include <string_view>

#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_FUNCTION_CALL_H_
#define MYCC_SOURCE_AST_STATEMENT_FUNCTION_CALL_H_
namespace Mycc::AST {
class FunctionCall : public ASTNode {
  public:
    FunctionCall(const Lexical::Token& name,
                 const std::shared_ptr<Type>& return_type,
                 std::list<std::unique_ptr<ASTNode>> args);

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::string _name;
    std::shared_ptr<Type> _return_type;
    std::list<std::unique_ptr<ASTNode>> _args;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_FUNCTION_CALL_H_