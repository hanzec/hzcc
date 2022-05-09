//
// Created by chen_ on 2022/3/29.
//
#include "return.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"

namespace Hzcc::AST {
const char* ReturnNode::NodeName() const { return "ReturnNode"; }
ReturnNode::ReturnNode(const Lexical::Token& token,
                       std::unique_ptr<ASTNode> return_val)
    : ASTNode(token.Location()), _return_val(std::move(return_val)) {}

std::string ReturnNode::PrintAdditionalInfo(const std::string& ident) const {
    std::stringstream info;

    info << _return_val->GetType()->GetName();

    // print node if available
    if (_return_val) {
        info << "\n" + _return_val->Dump(ident + " `");
    }
    return info.str();
}
Status ReturnNode::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& ReturnNode::GetReturnVal() { return _return_val; };

}  // namespace Hzcc::AST
