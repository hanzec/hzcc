//
// Created by chen_ on 2022/3/29.
//
#include "array.h"

#include <cassert>

#include "AST/type/array_type.h"
#include "lexical/Token.h"
namespace Mycc::AST {

std::string AST::ArraySubscriptExpr::GetNodeName() const {
    return "ArraySubscriptExpr";
}
std::shared_ptr<Type> ArraySubscriptExpr::GetType() const {
    return dynamic_cast<AST::ArrayType*>(_name->GetType().get())->GetBaseType();
}

std::string ArraySubscriptExpr::PrintAdditionalInfo(
    std::string_view ident) const {
    std::string result = std::string();

    result += GetType()->GetName() + "\n";

    result += _name->Dump(std::string(ident) + " |") + "\n";
    result += _index_expr->Dump(std::string(ident) + " `");

    return result;
}
ArraySubscriptExpr::ArraySubscriptExpr(const Lexical::Token& token,
                                       std::unique_ptr<AST::ASTNode> name,
                                       std::unique_ptr<AST::ASTNode> index)
    : ASTNode(token.Location()),
      _index_expr(std::move(index)),
      _name(std::move(name)) {
    assert(_name->GetType()->IsArray());
}
bool ArraySubscriptExpr::IsAssignable() const { return true; };

}  // namespace Mycc::AST