//
// Created by chen_ on 2022/3/26.
//
#include <list>
#include <string>
#include <string_view>
#include <utility>

#include "AST/type/Type.h"
#include "decl.h"
#ifndef MYCC_SOURCE_AST_STATEMENT_VALUE_DECL_H_
#define MYCC_SOURCE_AST_STATEMENT_VALUE_DECL_H_

namespace Hzcc::AST {
class VarDecl : public DeclNode {
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
    explicit VarDecl(std::shared_ptr<Type> type,
                     std::list<Lexical::Token>& attrs,
                     const Lexical::Token& decl_name,
                     std::unique_ptr<AST::ASTNode> init);

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
     * @brief Get the type of current VarDecl
     * @return the type of current VarDecl
     */
    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

    /**
     * @brief Get the init AST Node of current VarDecl Node
     * @note will exit(-1) if current VarDecl doesn't contain init value when
     * compile as DEBUG mode
     * @return the init AST Node of current VarDecl Node
     */
    [[nodiscard]] std::unique_ptr<AST::ASTNode>& GetInitExpr();

    /**
     * @brief Get the name of current Node
     * @return "VarDecl"
     */
    [[nodiscard]] const char* NodeName() const override;

  protected:
    [[nodiscard]] std::string PrintAdditionalInfo(
        const std::string& ident) const override;

  private:
    std::shared_ptr<Type> _type;
    std::unique_ptr<AST::ASTNode> _init_expr;
};

}  // namespace Hzcc::AST
#endif  // MYCC_SOURCE_AST_STATEMENT_VALUE_DECL_H_
