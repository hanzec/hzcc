//
// Created by chen_ on 2022/3/26.
//
#include <list>

#include "AST/type/Type.h"
#include "DeclStmt.h"
#ifndef HZCC_AST_STATEMENTS_VAR_DECL_H
#define HZCC_AST_STATEMENTS_VAR_DECL_H

namespace Hzcc::AST {
class VarDecl : public DeclStmt {
  public:
    /**
     * @brief Construct a new Var Decl object
     * @note this constructor will causing exit(-1) if type is nullptr when
     * compiling in debug mode
     * @param type The type of the variable
     * @param attrs The attributes of the variable
     * @param decl_name The name of the variable
     * @param init The initial value of the variable (optional)
     */
    VarDecl(const Position& loc, std::shared_ptr<Type> type,
            const std::string_view& name,
            std::unique_ptr<ASTNode> init);  // NOLINT

    /**
     * @brief visitor for AST
     * @param visitor the AST visitor
     * @return visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Inquire if current VarDecl contains init value or not
     * @return true if contains init value, false otherwise
     */
    [[nodiscard]] bool HasInitExpr() const override;

    /**
     * @brief Get the init AST Node of current VarDecl Node
     * @note will exit(-1) if current VarDecl doesn't contain init value when
     * compile as DEBUG mode
     * @return the init AST Node of current VarDecl Node
     */
    [[nodiscard]] std::unique_ptr<AST::ASTNode>& GetInitExpr();

  protected:
    /**
     * @brief Get Name of the node
     * @return will always return "VarDecl"
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
    std::unique_ptr<AST::ASTNode> _init_expr;
};

}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENTS_VAR_DECL_H
