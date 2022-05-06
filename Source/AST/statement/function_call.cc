//
// Created by chen_ on 2022/3/29.
//
#include "function_call.h"

#include <cassert>

#include "lexical/Token.h"

namespace Hzcc::AST {
const char* FunctionCall::GetNodeName() const { return "FunctionCall"; }
FunctionCall::FunctionCall(const Lexical::Token& name,
                           const std::shared_ptr<Type>& return_type,
                           std::list<std::unique_ptr<ASTNode>> args)
    : ASTNode(name.Location()),
      _name(name.Value()),
      _args(std::move(args)),
      _return_type(return_type) {
    DLOG_ASSERT(return_type != nullptr) << "return type is nullptr";
}

std::shared_ptr<Type> FunctionCall::GetType() const { return _return_type; }

std::string FunctionCall::PrintAdditionalInfo(std::string_view ident) const {
    std::string info{_name};

    if (!_args.empty()) {
        for (auto& arg : _args) {
            info += "\n" + arg->Dump(std::string(ident) +
                                     (arg == _args.back() ? " `" : " |"));
        }
    }
    return info;
}
Status FunctionCall::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::list<std::unique_ptr<ASTNode>>& FunctionCall::GetArgsNode() {
    return _args;
}
const std::string& FunctionCall::FuncName() const { return _name; };

}  // namespace Hzcc::AST
