//
// Created by chen_ on 2022/3/29.
//
#include "return.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"

namespace Hzcc::AST {
const char* ReturnNode::GetNodeName() const { return "ReturnNode"; }
ReturnNode::ReturnNode(const Lexical::Token& token,
                       std::unique_ptr<ASTNode> return_val)
    : ASTNode(token.Location()), _return_val(std::move(return_val)) {}

std::string ReturnNode::PrintAdditionalInfo(std::string_view ident) const {
    std::string info{_return_val->GetType()->GetName()};
    // print node if available
    if (_return_val) {
        info +=
            "\n" + _return_val->Dump(std::string(ident.length(), ' ') + " `");
    }
    return info;
}
Status ReturnNode::visit(ASTVisitor& visitor) { return visitor.visit(this); };

}  // namespace Hzcc::AST
