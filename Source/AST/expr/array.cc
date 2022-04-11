//
// Created by chen_ on 2022/3/29.
//
#include "array.h"

#include <glog/logging.h>

#include <cassert>

#include "AST/type/ArrayType.h"
#include "lexical/Token.h"
#include "options.h"
namespace Mycc::AST {

std::string AST::ArraySubscriptExpr::GetNodeName() const {
    return "ArraySubscriptExpr";
}
std::shared_ptr<Type> ArraySubscriptExpr::GetType() const {
    return std::dynamic_pointer_cast<AST::ArrayType>(_name->GetType())
        ->GetBaseType();
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
    if (Options::Global_enable_type_checking) {
        DLOG_IF(FATAL, !_name->GetType()->IsArray())
            << "ArraySubscriptExpr: " + _name->Dump("") +
                   " should have array type";
    }
}

bool ArraySubscriptExpr::IsAssignable() const { return true; }
void ArraySubscriptExpr::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
};

}  // namespace Mycc::AST
