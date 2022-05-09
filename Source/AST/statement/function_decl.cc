//
// Created by chen_ on 2022/3/27.
//
#include "function_decl.h"

#include <iomanip>

#include "AST/statement/compound.h"
#include "lexical/Token.h"
#include "utils/logging.h"

namespace Hzcc::AST {

FunctionDeclNode::FunctionDeclNode(const Lexical::Token& function_name,
                                   std::shared_ptr<Type> return_type,
                                   std::list<Lexical::Token>& attribute)
    : DeclNode(function_name), _return_type(std::move(return_type)) {
    attribute.clear();  // TODO need modify
}
bool FunctionDeclNode::set_body(
    std::unique_ptr<AST::CompoundStmt> declaration) {
    if (declaration == nullptr) {
        DLOG(WARNING) << "function body is nullptr";
        return false;
    }
    DVLOG(AST_LOG_LEVEL) << "set function body for function [" << GetName()
                         << "]";
    _function_body = std::move(declaration);
    return true;
}

std::string FunctionDeclNode::PrintAdditionalInfo(
    const std::string& ident) const {
    std::stringstream result;
    result << _return_type->GetName() + " " + GetName();

    // print its arguments
    result << " (";
    for (const auto& arg : _function_param) {
        result << arg->GetType()->Dump();

        if (arg != _function_param.back()) {
            result << ",";
        }
    }
    result << ")";

    // print argument node
    if (!_function_param.empty()) {
        for (const auto& arg : _function_param) {
            result << "\n" +
                          arg->Dump(ident + (arg == _function_param.back() &&
                                                     _function_body == nullptr
                                                 ? " `"
                                                 : " |")) +
                          (arg == _function_param.back() ? "" : "\n");
        }
    }

    // print its body
    if (_function_body != nullptr) {
        result << "\n" + _function_body->Dump(ident + " `");
    } else {
        result << ")";
    }
    return result.str();
}
bool FunctionDeclNode::AddFunctionArgument(
    std::unique_ptr<ParamVarDecl> param_var_decl) {
    if (param_var_decl == nullptr) {
        return false;
    }
    DVLOG(AST_LOG_LEVEL) << "Add argument " << param_var_decl->GetName() << "("
                         << param_var_decl->GetType()->Dump()
                         << ") to function node [" << GetName() << "]";
    _function_param.push_back(std::move(param_var_decl));
    return true;
}
const char* FunctionDeclNode::NodeName() const { return "FunctionDecl"; }
ArgumentList FunctionDeclNode::getArguments() {
    ArgumentList result;
    for (const auto& arg : _function_param) {
        result.push_back(
            std::make_tuple(arg->GetName(), arg->GetType(), arg->GetLine()));
    }
    return result;
}

bool FunctionDeclNode::IsFuncDecl() const { return true; }

std::shared_ptr<Type> FunctionDeclNode::GetType() const { return _return_type; }

bool FunctionDeclNode::HasBody() const { return _function_body != nullptr; }

Status FunctionDeclNode::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}

std::unique_ptr<AST::CompoundStmt>& FunctionDeclNode::Body() {
    return _function_body;
}
std::list<std::unique_ptr<ParamVarDecl>>& FunctionDeclNode::GetParams() {
    return _function_param;
}

#ifdef NDEBUG
std::string FunctionDeclNode::Dump(const std::string& ident) const {
    std::stringstream ret;

    // print parameter
    for (const auto& arg : _function_param) {
        auto type_name = arg->GetType()->GetName();
        if (type_name.find('[') != std::string::npos) {
            ret << "\tLine " << std::setw(3) << std::to_string(GetLine() + 1)
                << std::setw(0)
                << ": parameter " +
                       arg->GetType()->GetName().substr(
                           0, type_name.find_first_of('[')) +
                       " " + arg->GetName() +
                       arg->GetType()->GetName().substr(
                           type_name.find_first_of('['),
                           type_name.find_last_of(']')) +
                       "\n";
        } else {
            ret << "\tLine " << std::setw(3) << std::to_string(GetLine() + 1)
                << std::setw(0)
                << ": parameter " + arg->GetType()->GetName() + " " +
                       arg->GetName() + "\n";
        }
    }

    // print body
    if (_function_body != nullptr) {
        ret << _function_body->Dump(ident) + "\n";
    }

    return ret.str();
}
#endif

}  // namespace Hzcc::AST
