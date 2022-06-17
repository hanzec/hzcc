//
// Created by chen_ on 2022/3/29.
//
#include "VarDecl.h"

#include <utility>

#include "AST/utils/macro.h"
#include "options.h"
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
        << HZCC_AST_PRINT_NODE_INFO("init expression is nullptr", this);
}

const char* VarDecl::NodeName() const { return "VarDecl"; }

void VarDecl::PrintDetail(std::ostream& out, const std::string& ident) const {
    if (Options::Global_disable_color) {
        out << '\'' << GetName() << "' '" << RetType()->Name() << '\'';
    } else {
        out << KEnableBrightCyan << '\'' << GetName() << "\' " << KEnableGreen
            << '\'' << RetType()->Name() << '\'' << KDisableColor;
    }
    if (HasInitExpr()) _init_expr->Dump(out, ident + " `");
}
Status VarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }

bool VarDecl::HasInitExpr() const { return !_init_expr->IsEmptyStmt(); }

std::unique_ptr<AST::ASTNode>& VarDecl::GetInitExpr() {
    DLOG_ASSERT(_init_expr != nullptr) << "init expr is nullptr";
    return _init_expr;
}
}  // namespace Hzcc::AST
