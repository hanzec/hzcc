//
// Created by chen_ on 2022/3/24.
//

#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_RETURN_H_
#define MYCC_SOURCE_AST_STATEMENT_RETURN_H_
namespace Hzcc::AST {
class ReturnNode : public ASTNode {
  public:
    explicit ReturnNode(const Lexical::Token& token,
                        std::unique_ptr<ASTNode> return_val);

    Status visit(ASTVisitor& visitor) override;

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(std::string_view ident) const override {
        return "";
    }
#endif

    [[nodiscard]] bool IsReturn() const override { return true; }

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::unique_ptr<ASTNode> _return_val;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_RETURN_H_
