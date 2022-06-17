//
// Created by chen_ on 2022/6/13.
//
#include "AST/cast/expr/CastExpr.h"
#ifndef MYCC_SOURCE_AST_EXPR_IMPLICITCASTEXPR_H_
#define MYCC_SOURCE_AST_EXPR_IMPLICITCASTEXPR_H_
namespace Hzcc::AST {
class ImplicitCastExpr : public CastExpr {
  public:
    /**
     * @brief CastExpr constructor. Will cast expression to the given
     * to_type. If runtime assert is enabled, will trigger an error if the given
     * to_type is same as the expression's to_type.
     * @param to_type the to_type to cast to
     * @param expr the expression to cast
     * @param location location of the cast operator
     */
    ImplicitCastExpr(std::string name,                // NOLINT
                     const Position& location,        // NOLINT
                     std::unique_ptr<ASTNode> expr);  // NOLINT

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
    const std::string _cast_name;
};
}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_EXPR_IMPLICITCASTEXPR_H_
