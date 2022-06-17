//
// Created by chen_ on 2022/6/13.
//
#include "CastExpr.h"
#ifndef MYCC_SOURCE_AST_EXPR_CAST_CSTYLECASTEXPR_H_
#define MYCC_SOURCE_AST_EXPR_CAST_CSTYLECASTEXPR_H_
namespace Hzcc::AST {
class ExplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * to_type. If runtime assert is enabled, will trigger an error if the given
     * to_type is same as the expression's to_type.
     * @param to_type the to_type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     */
    ExplicitCastExpr(const Position& location,        // NOLINT
                   std::shared_ptr<Type> to_type,   // NOLINT
                   std::unique_ptr<ASTNode> expr);  // NOLINT


    /**
     * @brief return the type that is being casted to
     * @return the type that is being casted to
     */
    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "CastExpr"
     */
    [[nodiscard]] const char* NodeName() const override;

    /**
     * @brief An override function using print extra information when call
     * ASTNode->Dump()
     *     // TODO add details of printed information
     * @param ident the current indentation level
     */
    void PrintDetail(std::ostream& out,
                     const std::string& ident) const override;
  private:
    const std::shared_ptr<Type> _cast_type;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_EXPR_CAST_CSTYLECASTEXPR_H_
