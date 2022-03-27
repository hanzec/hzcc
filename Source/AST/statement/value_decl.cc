//
// Created by chen_ on 2022/3/29.
//
#include "value_decl.h"

#include <memory>
#include <utility>


#include "lexical/Token.h"
namespace Mycc::AST {

VarDecl::VarDecl(std::shared_ptr<Type>  type,
                 std::list<Lexical::Token>& attrs,
                 const Lexical::Token& decl_name)
    : DeclNode(decl_name), _type(std::move(type)) {
    attrs.clear();
    // TODO need to check attrs
}
std::string VarDecl::GetNodeName() const { return "VarDecl"; }
std::string VarDecl::PrintAdditionalInfo(std::string_view ident) const {
    return GetName() + " " + _type->GetName();
}
std::shared_ptr<Type> VarDecl::GetType() const { return _type; }
}  // namespace Mycc::AST