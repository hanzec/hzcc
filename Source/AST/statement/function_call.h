//
// Created by chen_ on 2022/3/27.
//
#include <list>
#include <memory>
#include <string>
#include <string_view>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_FUNCTION_CALL_H_
#define MYCC_SOURCE_AST_STATEMENT_FUNCTION_CALL_H_
namespace Hzcc::AST {
class FunctionCall : public ASTNode {
  public:
    FunctionCall(const Lexical::Token& name,
                 const std::shared_ptr<Type>& return_type,
                 std::list<std::unique_ptr<ASTNode>> args);

    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] const std::string& FuncName() const;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    [[nodiscard]] std::list<std::unique_ptr<ASTNode>>& GetArgsNode();

  protected:
    [[nodiscard]] const char* GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::string _name;
    std::shared_ptr<Type> _return_type;
    std::list<std::unique_ptr<ASTNode>> _args;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_FUNCTION_CALL_H_
