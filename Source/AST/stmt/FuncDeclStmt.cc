//
// Created by chen_ on 2022/3/27.
//
#include "FuncDeclStmt.h"

#include "AST/stmt/CompoundStmt.h"
#include "AST/utils/macro.h"

namespace Hzcc::AST {

FuncDeclStmt::FuncDeclStmt(const Position& location,
                           const std::string_view& name,
                           std::shared_ptr<Type> return_type)  // NO_LINT
    : DeclStmt(std::move(return_type), name, location) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    // name and type is checked in DeclStmt
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

void FuncDeclStmt::PrintDetail(std::ostream& out,
                               const std::string& ident) const {
    out << RetType()->GetName() + " " + GetName();

    // print its arguments
    out << " (";
    for (const auto& arg : _function_param) {
        out << arg->RetType()->Dump();

        if (arg != _function_param.back()) {
            out << ",";
        }
    }
    out << ")";

    // print argument node
    if (!_function_param.empty()) {
        for (const auto& arg : _function_param) {
            arg->Dump(out, ident + (arg == _function_param.back() &&
                                            _function_body == nullptr
                                        ? " `"
                                        : " |"));
        }
    }

    // print its body
    if (_function_body != nullptr) {
        _function_body->Dump(out, ident + " `");
    } else {
        out << ")";
    }
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

bool FuncDeclStmt::HasBody() const { return _function_body != nullptr; }

Status FuncDeclStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }

std::unique_ptr<AST::CompoundStmt>& FuncDeclStmt::Body() {
    return _function_body;
}
std::list<std::unique_ptr<ParamVarDecl>>& FuncDeclStmt::GetParams() {
    return _function_param;
}
}  // namespace Hzcc::AST
