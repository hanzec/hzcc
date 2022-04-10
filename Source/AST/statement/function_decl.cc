//
// Created by chen_ on 2022/3/27.
//
#include "function_decl.h"

#include <iomanip>
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
    DVLOG(AST_LOG_LEVEL) << "set function body for function [" << GetName()
                         << "]";
    _function_body = std::move(declaration);
    return true;
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
    result += ")";

    // print argument node
    if (!_function_param.empty()) {
        for (const auto& arg : _function_param) {
            result += "\n" + arg->Dump(std::string(ident.size(), ' ') + " |") +
                      (arg == _function_param.back() ? "" : "\n");
        }
    }

    // print its body
    if (_function_body != nullptr) {
        result += "\n" +
                  _function_body->Dump(std::string(ident.size(), ' ') +
                                       (_function_param.empty() ? " |" : " `"));
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

void FunctionDeclNode::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

std::unique_ptr<AST::ASTNode>& FunctionDeclNode::GetBody() {
    return _function_body;
}

#ifdef NDEBUG
std::string FunctionDeclNode::Dump(std::string_view ident) const {
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
        ret << _function_body->Dump(ident);
    }

    return ret.str();
}
#endif

}  // namespace Mycc::AST