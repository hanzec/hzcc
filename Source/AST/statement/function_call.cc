//
// Created by chen_ on 2022/3/29.
//
#include <cassert>
#include "function_call.h"

#include "lexical/Token.h"

namespace Mycc::AST {
std::string FunctionCall::GetNodeName() const { return "FunctionCall"; }
FunctionCall::FunctionCall(const Lexical::Token& name,
                           const std::shared_ptr<Type>& return_type,
                           std::list<std::unique_ptr<ASTNode>> args)
    : ASTNode(name.Location()),
      _name(name.Value()),
      _args(std::move(args)),
      _return_type(return_type) {
    assert(return_type);
}

std::shared_ptr<Type> FunctionCall::GetType() const { return _return_type; }

std::string FunctionCall::PrintAdditionalInfo(std::string_view ident) const {
    std::string info{_name};

    if (!_args.empty()) {
        for (auto& arg : _args) {
            info += "\n" + arg->Dump(std::string(ident) + " |");
        }
    }
    return ASTNode::PrintAdditionalInfo(ident);
};

}  // namespace Mycc::AST
