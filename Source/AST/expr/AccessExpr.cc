//
// Created by Hanze Chen on 2022/3/29.
//
#include "AccessExpr.h"

#include "AST/type/StructType.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
AccessExpr::AccessExpr(bool isPtr, const Position& location,
                       const std::string_view& field,
                       std::unique_ptr<ASTNode> chain_access)  // NOLINT
    : ASTNode(location),
      _ptr_access(isPtr),
      _field(std::string(field)),
      _varaible(std::move(chain_access)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!field.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("field string empty", this);
    HZCC_RUNTIME_CHECK(_varaible != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("chain_access is nullptr", this);
    HZCC_RUNTIME_CHECK(_varaible->RetType()->IsStruct())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("chain_access is not struct", this);
}

const char* AST::AccessExpr::NodeName() const { return "AccessExpr"; }

bool AccessExpr::IsReturnLValue() const { return true; }

std::shared_ptr<Type> AccessExpr::RetType() const {
    return dynamic_cast<StructType*>(_varaible->RetType().get())
        ->GetChild(_field);
}
Status AccessExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
