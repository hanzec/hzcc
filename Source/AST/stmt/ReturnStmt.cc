//
// Created by chen_ on 2022/3/29.
//
#include "ReturnStmt.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {
ReturnStmt::ReturnStmt(std::unique_ptr<ASTNode> expr,  // NOLINT
                       const Position& loc)            // NOLINT
    : ASTNode(loc), _return_val(std::move(expr)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("return statement is nullptr", this);
}

const char* ReturnStmt::NodeName() const { return "ReturnStmt"; }

std::string ReturnStmt::PrintDetail(const std::string& ident) const {
    std::stringstream info;

    info << _return_val->RetType()->GetName();

    // print node if available
    if (_return_val) {
        info << "\n" + _return_val->Dump(ident + " `");
    }
    return info.str();
}
Status ReturnStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& ReturnStmt::GetReturnVal() { return _return_val; };

}  // namespace Hzcc::AST
