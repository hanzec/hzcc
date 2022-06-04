//
// Created by chen_ on 2022/3/27.
//
#include "FuncDeclStmt.h"

#include "AST/stmt/CompoundStmt.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {

FuncDeclStmt::FuncDeclStmt(const std::string_view& name,         // NO_LINT
                           std::shared_ptr<Type> return_type,    // NO_LINT
                           const std::pair<int, int>& location)  // NO_LINT
    : DeclStmt(name, location), _return_type(std::move(return_type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    // name is checked in DeclStmt
    HZCC_RUNTIME_CHECK(_return_type != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("return type is nullptr", this);
}

bool FuncDeclStmt::set_body(std::unique_ptr<AST::CompoundStmt> declaration) {
    if (declaration == nullptr) {
        DLOG(WARNING) << "function body is nullptr";
        return false;
    }
    DVLOG(AST_LOG_LEVEL) << "set function body for function [" << GetName()
                         << "]";
    _function_body = std::move(declaration);
    return true;
}

std::string FuncDeclStmt::PrintDetail(const std::string& ident) const {
    std::stringstream result;
    result << _return_type->GetName() + " " + GetName();

    // print its arguments
    result << " (";
    for (const auto& arg : _function_param) {
        result << arg->RetType()->Dump();

        if (arg != _function_param.back()) {
            result << ",";
        }
    }
    result << ")";

    // print argument node
    if (!_function_param.empty()) {
        result << "\n";
        for (const auto& arg : _function_param) {
            result << arg->Dump(ident + (arg == _function_param.back() &&
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
bool FuncDeclStmt::AddFunctionArgument(
    std::unique_ptr<ParamVarDecl> param_var_decl) {
    if (param_var_decl == nullptr) {
        return false;
    }
    DVLOG(AST_LOG_LEVEL) << "Add argument " << param_var_decl->GetName() << "("
                         << param_var_decl->RetType()->Dump()
                         << ") to function node [" << GetName() << "]";
    _function_param.push_back(std::move(param_var_decl));
    return true;
}
const char* FuncDeclStmt::NodeName() const { return "FunctionDecl"; }

ArgumentList FuncDeclStmt::getArguments() {
    ArgumentList result;
    for (const auto& arg : _function_param) {
        result.push_back(std::make_tuple(arg->GetName(), arg->RetType(),
                                         arg->Location().first));
    }
    return result;
}

bool FuncDeclStmt::IsFuncDecl() const { return true; }

std::shared_ptr<Type> FuncDeclStmt::RetType() const { return _return_type; }

bool FuncDeclStmt::HasBody() const { return _function_body != nullptr; }

Status FuncDeclStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }

std::unique_ptr<AST::CompoundStmt>& FuncDeclStmt::Body() {
    return _function_body;
}
std::list<std::unique_ptr<ParamVarDecl>>& FuncDeclStmt::GetParams() {
    return _function_param;
}
}  // namespace Hzcc::AST
