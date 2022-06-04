//
// Created by chen_ on 2022/3/29.
//
#include "DeclRefExpr.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {
DeclRefExpr::DeclRefExpr(std::shared_ptr<Type> type,      // NO_LINT
                         const std::string_view& name,    // NO_LINT
                         const std::pair<int, int>& loc)  // NO_LINT
    : ASTNode(loc), _name(std::string(name)), _type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(type != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type is nullptr", this);
    HZCC_RUNTIME_CHECK(!name.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("name is empty string", this);
}

const char* DeclRefExpr::NodeName() const { return "DeclRefExpr"; }

std::shared_ptr<Type> DeclRefExpr::RetType() const { return _type; }

std::string DeclRefExpr::PrintDetail(const std::string& ident) const {
    return _name + " " + _type->GetName();
}
bool DeclRefExpr::IsReturnLValue() const { return true; }
Status DeclRefExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
const std::string& DeclRefExpr::VarName() const { return _name; }

}  // namespace Hzcc::AST
