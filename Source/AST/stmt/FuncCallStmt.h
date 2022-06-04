//
// Created by chen_ on 2022/3/27.
//
#include <list>

#include "AST/ASTNode.h"

#ifndef HZCC_AST_STATEMENT_FUNC_CALL_H
#define HZCC_AST_STATEMENT_FUNC_CALL_H
namespace Hzcc::AST {
class FuncCallStmt : public ASTNode {
  public:
    /**
     * @brief Constructor of FuncCallStmt
     * @param type type of the function call
     * @param name name of the function call
     * @param location location of the function call
     * @param args arguments of the function call
     */
    FuncCallStmt(const Position& location, std::shared_ptr<Type> type,
                 const std::string_view& name,
                 std::list<std::unique_ptr<ASTNode>> args);  // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    [[nodiscard]] const std::string& FuncName() const;

    [[nodiscard]] std::shared_ptr<Type> RetType() const override;

    [[nodiscard]] const std::list<std::unique_ptr<ASTNode>>& GetArgsNode();

    /**
     * @brief Get Name of the node
     * @return will always return "FuncCallStmt"
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
    const std::string _name;
    const std::shared_ptr<Type> _return_type;
    const std::list<std::unique_ptr<ASTNode>> _args;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_FUNC_CALL_H
