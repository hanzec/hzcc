//
// Created by chen_ on 2022/3/29.
//
#include "access.h"

#include <cassert>

#include "AST/type/struct_type.h"
#include "lexical/Token.h"
#include "options.h"
namespace Mycc::AST {

std::string AST::AccessExpr::GetNodeName() const { return "AccessExpr"; }
AccessExpr::AccessExpr(bool isPtr, const Lexical::Token& token,
                       std::unique_ptr<ASTNode> expr)
    : ASTNode(token.Location()),
      _ptr_access(isPtr),
      _varaible(std::move(expr)),
      _field(std::move(token.Value())) {
    if (Options::Global_enable_type_checking) {
        assert(_varaible->GetType()->IsStruct());
    }
}

bool AccessExpr::IsAssignable() const { return true; }

std::shared_ptr<Type> AccessExpr::GetType() const {
    return dynamic_cast<StructType*>(_varaible->GetType().get())
        ->GetChild(_field);
}
}  // namespace Mycc::AST