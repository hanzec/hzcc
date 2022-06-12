//
// Created by chen_ on 2022/3/29.
//
#include "ReturnStmt.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {
ReturnStmt::ReturnStmt(const Position& loc,            // NOLINT
                       std::unique_ptr<ASTNode> expr)  // NOLINT
    : ASTNode(loc), _return_val(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_return_val != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("return statement is nullptr", this);
}

const char* ReturnStmt::NodeName() const { return "ReturnStmt"; }

void ReturnStmt::PrintDetail(std::ostream& out,
                             const std::string& ident) const {
    out << _return_val->RetType()->GetName();

    // print node if available
    if (_return_val) {
        _return_val->Dump(out, ident + " `");
    }
}
Status ReturnStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& ReturnStmt::GetReturnVal() { return _return_val; }
std::shared_ptr<Type> ReturnStmt::RetType() const {
    return _return_val->RetType();
};

}  // namespace Hzcc::AST
