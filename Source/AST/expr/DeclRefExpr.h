#include "AST/ASTNode.h"

#ifndef HZCC_AST_EXPR_DECL_REF_EXPR_H
#define HZCC_AST_EXPR_DECL_REF_EXPR_H
namespace Hzcc::AST {
class DeclRefExpr : public ASTNode {
  public:
    /**
     * @brief Constructor of DeclRefExpr. If runtime assert is enabled, will
     * trigger an error if the given name is empty string.
     * @param loc location of the node
     * @param type type of the variable
     * @param name The name of the variable.
     */
    DeclRefExpr(const Position& loc, std::shared_ptr<Type> type,
                const std::string_view& name);  // NO_LINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief determine whether the node will return a location value (LValue)
     * or register value (RValue)
     * @return true if the node will return a LValue, false otherwise
     */
    [[nodiscard]] bool IsReturnLValue() const override;

    /**
     * @brief get the name of the variable
     * @return the name of the variable
     */
    [[nodiscard]] const std::string& VarName() const;

    /**
     * @brief get the type of the variable
     * @return the type of the variable
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "DeclRefExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     * @return std::string generated string
     */
    [[nodiscard]] std::string PrintDetail(
        const std::string& ident) const override;

  private:
    const std::string _name;
    const std::shared_ptr<Type> _type;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_EXPR_DECL_REF_EXPR_H
