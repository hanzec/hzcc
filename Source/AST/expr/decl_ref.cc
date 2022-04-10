//
// Created by chen_ on 2022/3/29.
//
#include "decl_ref.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"

namespace Mycc::AST {
std::string DeclRefExpr::GetNodeName() const { return "DeclRefExpr"; }
DeclRefExpr::DeclRefExpr(const Lexical::Token& name, std::shared_ptr<Type> type)
    : ASTNode(name.Location()), _name(name.Value()), _type(std::move(type)) {}

std::shared_ptr<Type> DeclRefExpr::GetType() const { return _type; }

std::string DeclRefExpr::PrintAdditionalInfo(std::string_view ident) const {
    return _name;
}
bool DeclRefExpr::IsAssignable() const { return true; }
void DeclRefExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

}  // namespace Mycc::AST
