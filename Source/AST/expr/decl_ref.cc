//
// Created by chen_ on 2022/3/29.
//
#include "decl_ref.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"

namespace Hzcc::AST {
const char* DeclRefExpr::GetNodeName() const { return "DeclRefExpr"; }
DeclRefExpr::DeclRefExpr(const Lexical::Token& name, std::shared_ptr<Type> type)
    : ASTNode(name.Location()), _name(name.Value()), _type(std::move(type)) {}

std::shared_ptr<Type> DeclRefExpr::GetType() const { return _type; }

std::string DeclRefExpr::PrintAdditionalInfo(std::string_view ident) const {
    return _name + " " + _type->GetName();
}
bool DeclRefExpr::IsAssignable() const { return true; }
Status DeclRefExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
const std::string& DeclRefExpr::VarName() const { return _name; }

}  // namespace Hzcc::AST
