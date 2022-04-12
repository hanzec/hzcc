//
// Created by chen_ on 2022/3/25.
//
#include <memory>

#include "AST/ASTNode.h"

#ifndef MYCC_SOURCE_AST_STATEMENT_FOR_H_
#define MYCC_SOURCE_AST_STATEMENT_FOR_H_
namespace Mycc::AST {
class ForStatement : public ASTNode {
  public:
    ForStatement(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond,
                 std::unique_ptr<ASTNode> step, std::unique_ptr<ASTNode> body,
                 std::pair<int, int> location)
        : ASTNode(location),
          _init(std::move(init)),
          _cond(std::move(cond)),
          _step(std::move(step)),
          _body(std::move(body)) {}

    void visit(ASTVisitor& visitor) override;

    [[nodiscard]] bool HasBody() const override { return true; }

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(std::string_view ident) const override;
#endif

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::unique_ptr<ASTNode> _init;
    std::unique_ptr<ASTNode> _cond;
    std::unique_ptr<ASTNode> _step;
    std::unique_ptr<ASTNode> _body;
};

};      // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_FOR_H_
