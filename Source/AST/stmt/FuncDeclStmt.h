#include <list>

#include "AST/ASTNode.h"
#include "AST/type/Type.h"
#include "DeclStmt.h"
#include "ParamVarDecl.h"

#ifndef HZCC_AST_STATEMENT_FUNC_DECL_STMT_H
#define HZCC_AST_STATEMENT_FUNC_DECL_STMT_H
namespace Hzcc::AST {
using ArgumentList =
    std::list<std::tuple<std::string, std::shared_ptr<AST::Type>, int>>;

class FuncDeclStmt : public DeclStmt {
  public:
    /**
     * @brief Construct a new Function Prototype For Function Node
     *
     * @param name the name of the function
     * @param return_type the return type of the function
     * @param location the location of the function
     */
    FuncDeclStmt(const Position& location, const std::string_view& name,
                 std::shared_ptr<Type> return_type);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    bool set_body(std::unique_ptr<AST::CompoundStmt> declaration);

    std::unique_ptr<AST::CompoundStmt>& Body();

    std::list<std::unique_ptr<ParamVarDecl>>& GetParams();

    ArgumentList getArguments();

    /**
     * @brief Determine whether the node have a body or not
     * @return true if the node have a body, false otherwise
     */
    [[nodiscard]] bool HasBody() const override;

    [[nodiscard]] bool IsFuncDecl() const override;

    bool AddFunctionArgument(std::unique_ptr<ParamVarDecl> type);

    /**
     * @brief Get Name of the node
     * @return will always return "FuncDeclStmt"
     */
    [[nodiscard]] const char* NodeName() const override;

  protected:
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
    std::unique_ptr<CompoundStmt> _function_body;
    std::list<std::unique_ptr<ParamVarDecl>> _function_param;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_FUNC_DECL_STMT_H
