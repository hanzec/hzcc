//
// Created by chen_ on 2022/3/29.
//
#include "value_decl.h"

#include <memory>
#include <utility>

#include "lexical/Token.h"
namespace Hzcc::AST {

VarDecl::VarDecl(std::shared_ptr<Type> type, std::list<Lexical::Token>& attrs,
                 const Lexical::Token& decl_name,
                 std::unique_ptr<AST::ASTNode> init)
    : DeclNode(decl_name), _type(std::move(type)), _init_expr(std::move(init)) {
    attrs.clear();
}
const char* VarDecl::GetNodeName() const { return "VarDecl"; }

std::string VarDecl::PrintAdditionalInfo(std::string_view ident) const {
    std::stringstream ss;
    // base information
    ss << GetName() << " " << _type->GetName();

    if (HasInitExpr())
        ss << std::endl << _init_expr->Dump(std::string(ident) + " `");

    return ss.str();
}
std::shared_ptr<Type> VarDecl::GetType() const { return _type; }

Status VarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }

bool VarDecl::HasInitExpr() const { return _init_expr != nullptr; }

std::unique_ptr<AST::ASTNode>& VarDecl::GetInitExpr() {
    DLOG_ASSERT(_init_expr != nullptr) << "init expr is nullptr";
    return _init_expr;
}
}  // namespace Hzcc::AST
