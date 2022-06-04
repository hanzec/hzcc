//
// Created by chen_ on 2022/3/29.
//
#include "VarDecl.h"

#include <utility>

#include "AST/utils/macro.h"
namespace Hzcc::AST {
VarDecl::VarDecl(const Position& loc, std::shared_ptr<Type> type,
                 const std::string_view& name,
                 std::unique_ptr<ASTNode> init)  // NOLINT
    : DeclStmt(std::move(type), name, loc), _init_expr(std::move(init)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    // name and type is checked in DeclStmt
    HZCC_RUNTIME_CHECK(_init_expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("init expression is nullptr", this);
}

const char* VarDecl::NodeName() const { return "VarDecl"; }

std::string VarDecl::PrintDetail(const std::string& ident) const {
    std::stringstream ss;
    // base information
    ss << GetName() << " " << RetType()->GetName();

    if (HasInitExpr()) ss << std::endl << _init_expr->Dump(ident + " `");

    return ss.str();
}
Status VarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }

bool VarDecl::HasInitExpr() const { return _init_expr != nullptr; }

std::unique_ptr<AST::ASTNode>& VarDecl::GetInitExpr() {
    DLOG_ASSERT(_init_expr != nullptr) << "init expr is nullptr";
    return _init_expr;
}
}  // namespace Hzcc::AST
