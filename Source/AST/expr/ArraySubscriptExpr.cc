//
// Created by chen_ on 2022/3/29.
//
#include "ArraySubscriptExpr.h"

#include "AST/type/ArrayType.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
ArraySubscriptExpr::ArraySubscriptExpr(
    std::unique_ptr<ASTNode> array,       // NOLINT
    std::unique_ptr<ASTNode> subscript,   // NOLINT
    const std::pair<int, int>& location)  // NOLINT
    : ASTNode(location),
      _array(std::move(array)),
      _subscript(std::move(subscript)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_array != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("array is nullptr", this);
    HZCC_RUNTIME_CHECK(_subscript != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("subscript is nullptr", this);
    HZCC_RUNTIME_CHECK(_array->RetType()->IsArray())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("not accessing array", this);
}

const char* AST::ArraySubscriptExpr::NodeName() const {
    return "ArraySubscriptExpr";
}
std::shared_ptr<Type> ArraySubscriptExpr::RetType() const {
    return std::dynamic_pointer_cast<AST::ArrayType>(_array->RetType())
        ->GetBaseType();
}

std::string ArraySubscriptExpr::PrintDetail(const std::string& ident) const {
    std::string result = std::string();

    result += RetType()->GetName() + "\n";

    result += _array->Dump(ident + " |") + "\n";
    result += _subscript->Dump(ident + " `");

    return result;
}

bool ArraySubscriptExpr::IsReturnLValue() const { return true; }

Status ArraySubscriptExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}

bool ArraySubscriptExpr::IsDereference() const { return true; }
std::unique_ptr<AST::ASTNode>& ArraySubscriptExpr::GetArrayBase() {
    return _array;
}
std::unique_ptr<AST::ASTNode>& ArraySubscriptExpr::GetSubscript() {
    return _subscript;
}
}  // namespace Hzcc::AST
