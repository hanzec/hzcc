//
// Created by chen_ on 2022/3/29.
//
#include "VarDecl.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
VarDecl::VarDecl(std::shared_ptr<Type> type,      // NOLINT
                 const std::string_view& name,    // NOLINT
                 std::unique_ptr<ASTNode> init,   // NOLINT
                 const std::pair<int, int>& loc)  // NOLINT
    : DeclStmt(name, loc), _type(std::move(type)), _init_expr(std::move(init)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    // name is checked in DeclStmt
    HZCC_RUNTIME_CHECK(_type != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type is nullptr", this);
    HZCC_RUNTIME_CHECK(_init_expr != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("init expression is nullptr", this);
}

const char* VarDecl::NodeName() const { return "VarDecl"; }

std::string VarDecl::PrintDetail(const std::string& ident) const {
    std::stringstream ss;
    // base information
    ss << GetName() << " " << _type->GetName();

    if (HasInitExpr()) ss << std::endl << _init_expr->Dump(ident + " `");

    return ss.str();
}
std::shared_ptr<Type> VarDecl::RetType() const { return _type; }

Status VarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }

bool VarDecl::HasInitExpr() const { return _init_expr != nullptr; }

std::unique_ptr<AST::ASTNode>& VarDecl::GetInitExpr() {
    DLOG_ASSERT(_init_expr != nullptr) << "init expr is nullptr";
    return _init_expr;
}
}  // namespace Hzcc::AST
