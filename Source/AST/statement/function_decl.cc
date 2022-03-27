//
// Created by chen_ on 2022/3/27.
//
#include "function_decl.h"

#include <ios>

#include "lexical/Token.h"
#include "utils/logging.h"

namespace Mycc::AST {

FunctionDeclNode::FunctionDeclNode(const Lexical::Token& function_name,
                                   std::shared_ptr<Type> return_type,
                                   std::list<Lexical::Token>& attribute)
    : DeclNode(function_name), _return_type(std::move(return_type)) {
    attribute.clear();  // TODO need modify
}
bool FunctionDeclNode::set_body(std::unique_ptr<AST::ASTNode> declaration) {
    if (declaration == nullptr) {
        DLOG(WARNING) << "function body is nullptr";
        return false;
    }
    DVLOG(AST_LOG_LEVEL) << "set function body for function " << GetName();
    _function_body = std::move(declaration);
    return true;
}

std::list<std::shared_ptr<Type>> FunctionDeclNode::get_argument() const {
    std::list<std::shared_ptr<Type>> result;
    for (const auto& arg : _function_param) {
        result.push_back(arg->GetType());
    }
    return result;
}
std::string FunctionDeclNode::PrintAdditionalInfo(
    std::string_view ident) const {
    std::string result = std::string();
    result += _return_type->GetName() + " " + GetName();

    // print its arguments
    result += " (";
    for (const auto& arg : _function_param) {
        result += arg->GetType()->Dump();
        result += ",";
    }
    if (!_function_param.empty()) {
        result.pop_back();
    }
    result += ")\n";

    // print argument node
    if (!_function_param.empty()) {
        for (const auto& arg : _function_param) {
            result += arg->Dump(std::string(ident.size(), ' ') + " |") + "\n";
        }
    }

    // print its body
    if (_function_body != nullptr) {
        result +=
            _function_body->Dump(std::string(ident.size(), ' ') +
                                 (_function_param.empty() ? " |" : " `")) +
            "\n";
    } else {
        result += ")";
    }

    return result;
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
std::string FunctionDeclNode::GetNodeName() const { return "FunctionDecl"; }
ArgumentList FunctionDeclNode::getArguments() {
    ArgumentList result;
    for (const auto& arg : _function_param) {
        result.push_back(std::pair(arg->GetName(), arg->GetType()));
    }
    return result;
}

bool FunctionDeclNode::IsFuncDecl() const { return true; }

std::shared_ptr<Type> FunctionDeclNode::GetType() const { return _return_type; }

bool FunctionDeclNode::HasBody() const { return _function_body != nullptr; }

#ifdef NDEBUG
std::string FunctionDeclNode::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    for (const auto& arg : _function_param) {
        ret += "\tLine " + std::to_string(GetLine()) + ": " + "parameter " +
               arg->GetType()->GetName() + " " + arg->GetName() + "\n";
    }

    // print body
    if (_function_body != nullptr) {
        ret += _function_body->Dump(ident) + "\n";
    }

    return ret;
}
#endif

}  // namespace Mycc::AST