//
// Created by chen_ on 2022/3/29.
//

#include "AST/type/Type.h"
#include "VarDecl.h"
#ifndef HZCC_AST_STATEMENT_PARAM_VAR_DECL_H
#define HZCC_AST_STATEMENT_PARAM_VAR_DECL_H
namespace Hzcc::AST {
class ParamVarDecl : public VarDecl {
  public:
    /**
     * @brief Constructor of ParamVarDecl
     * @param name name of the parameter
     * @param loc location of the parameter
     * @param type type of the parameter
     */
    ParamVarDecl(std::shared_ptr<Type> type,    // NOLINT
                 const std::string_view& name,  // NOLINT
                 const Position& loc);          // NOLINT

    /**
     * @brief AST Visitor acceptor
     * @param visitor Visitor object
     * @return return object of Hzcc::Status with content of visit result
     */
    Status visit(ASTVisitor& visitor) override;

    /**
     * @brief Get Name of the node
     * @return will always return "ParamVarDecl"
     */
    [[nodiscard]] const char* NodeName() const override;
};
}  // namespace Hzcc::AST
#endif  // HZCC_AST_STATEMENT_PARAM_VAR_DECL_H
