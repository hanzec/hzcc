//
// Created by Hanze Chen on 2022/2/3.
//

#include <memory>
#include <optional>
#include <string>

#include "ASTVisitor.h"
#include "macro.h"
#include "utils/logging.h"
#ifndef HZCC_AST_AST_NODE_H
#define HZCC_AST_AST_NODE_H
namespace Hzcc::AST {
class Type;
class CastExpr;
class DeduceValue;
class ASTNode {
  public:
    virtual ~ASTNode();

    explicit ASTNode(Position loc);

    /**
     * @brief The unique id of the node.
     * @return The unique id of the node.
     */
    [[nodiscard]] uint64_t Id() const;

    /**
     * @brief Get the location of this ASTNode in the source code. The location
     * have the form of (line, column).
     * @return The location of this ASTNode
     */
    [[nodiscard]] const Position& Location() const;

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] virtual bool HasBody() const { return false; }

    /**
     * @brief Determine whether the node is a literal node or not
     * @return true if the node is a literal node, false otherwise
     */
    [[nodiscard]] virtual bool IsLiteral() const { return false; }

    /**
     * @brief Determine whether the current AST node is a declaration node or
     * not
     * @return true if the node is a declaration node, false otherwise
     */
    [[nodiscard]] virtual bool IsDeclNode() const { return false; }

    /**
     * @brief Determine whether the current AST node is a struct declaration
     * node or not node or not
     * @return true if the node is a struct declaration node, false otherwise
     */
    [[nodiscard]] virtual bool IsStructDecl() const { return false; }

    /**
     * @brief Determine whether the current AST node is a empty statement node
     * or not
     * @return true if the node is a empty statement node, false otherwise
     */
    [[nodiscard]] virtual bool IsEmptyStmt() const { return false; }

    /**
     * @brief Determine whether the node is used to access element in array or
     * not.
     * @return
     */
    [[nodiscard]] virtual bool IsDereference() const { return false; }

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or register value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] virtual bool IsReturnLValue() const { return false; }

    /**
     * @brief Determine whether the node is a return statement or not
     * @return true if the node is a return statement, false otherwise
     */
    [[nodiscard]] virtual bool IsReturn() const { return false; }

    [[nodiscard]] static std::unique_ptr<AST::ASTNode> CastTo(
        const std::shared_ptr<Type>& lhs_type, std::unique_ptr<ASTNode> rhs);

    void Dump(std::ostream& out, const std::string& ident) const;

    [[nodiscard]] virtual std::optional<DeduceValue> GetDeducedValue() const;

    [[nodiscard]] virtual std::shared_ptr<Type> RetType() const;

    virtual Status visit(ASTVisitor& visitor) = 0;

    /**
     * @brief Get Name of the node
     * @return the name of the node
     */
    [[nodiscard]] virtual const char* NodeName() const = 0;

    std::ostream& operator<<(std::ostream& out) const;

  protected:
    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    virtual void PrintDetail(std::ostream& out, const std::string& ident) const;

  private:
    const uint64_t _id;
    const Position _node_location;
    inline static uint64_t _counter_ = 0;
};

}  // namespace Hzcc::AST

#endif  // HZCC_AST_AST_NODE_H
