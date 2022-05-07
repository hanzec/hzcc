//
// Created by chen_ on 2022/2/3.
//

#ifndef MYCC_EXPR_NODE_H
#define MYCC_EXPR_NODE_H

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "ASTVisitor.h"
#include "utils/logging.h"

namespace Hzcc {

namespace Lexical {
class Token;
}  // namespace Lexical

/**
 * @brief The base class of all AST nodes.
 */
namespace AST {
class Type;
class CastExpr;
class DeduceValue;
class ASTNode {
  public:
    virtual ~ASTNode();

    explicit ASTNode(std::pair<int, int> loc);

    [[nodiscard]] int GetLine() const { return _node_location.first; }

    [[nodiscard]] virtual bool HasBody() const { return false; }

    [[nodiscard]] virtual bool IsAssignable() const { return false; }

    [[nodiscard]] virtual bool IsDeclNode() const { return false; }

    [[nodiscard]] virtual bool IsStructDecl() const { return false; }

    /**
     * @brief Determine whether the node is used to access element in array or
     * not.
     * @return
     */
    [[nodiscard]] virtual bool IsDereference() const { return false; }

    [[nodiscard]] virtual bool IsEmptyStmt() const { return false; }


    [[nodiscard]] virtual bool IsLiteral() const { return false; }

    [[nodiscard]] virtual bool IsReturn() const { return false; }

    [[nodiscard]] static std::unique_ptr<AST::ASTNode> CastTo(
        const std::shared_ptr<Type>& lhs_type, std::unique_ptr<ASTNode> rhs);

    [[nodiscard]] virtual std::string Dump(std::string_view ident) const;

    [[nodiscard]] virtual std::optional<DeduceValue> GetDeducedValue() const;

    [[nodiscard]] virtual std::shared_ptr<Type> GetType() const;

    virtual Status visit(ASTVisitor& visitor) = 0;

    [[nodiscard]] std::pair<int, int> Location() const;

    [[nodiscard]] virtual const char* GetNodeName() const = 0;

    [[nodiscard]] uint64_t GetNodeId() const;

  protected:
    [[nodiscard]] virtual std::string PrintAdditionalInfo(
        std::string_view ident) const;

  private:
    const uint64_t _id;
    inline static uint64_t _counter_ = 0;
    std::pair<int, int> _node_location{-1, -1};
};

}  // namespace AST

}  // namespace Hzcc

#endif  // MYCC_EXPR_NODE_H
