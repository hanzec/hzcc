//
// Created by chen_ on 2022/3/29.
//
#include "return.h"
#include "lexical/Token.h"
#include "AST/type/type.h"

namespace Mycc::AST {
std::string ReturnNode::GetNodeName() const { return "ReturnNode"; }
ReturnNode::ReturnNode(const Lexical::Token& token,
                       std::unique_ptr<ASTNode> return_val)
    : ASTNode(token.Location()), _return_val(std::move(return_val)) {}

std::string ReturnNode::PrintAdditionalInfo(std::string_view ident) const {
    std::string info{_return_val->GetType()->GetName()};

    // print node if avaliable
    if (_return_val) {
        info += "\n" + _return_val->Dump(std::string(ident) + " `");
    }
    return info;
};

}  // namespace Mycc::AST
