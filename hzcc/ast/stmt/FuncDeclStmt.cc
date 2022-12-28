//
// Created by chen_ on 2022/3/27.
//
#include "ast/Stmt.h"

namespace hzcc::ast {

FuncDeclStmt::FuncDeclStmt(const Position& loc, std::string_view name,
                           std::shared_ptr<Type> return_type)  // NO_LINT
    : IDeclStmt("FunctionDecl", name, loc),
      _return_type(std::move(return_type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK({
        INTERNAL_LOG_IF(FATAL, _return_type != nullptr)
            << UniqueName() << "type is nullptr";
    })
}

bool FuncDeclStmt::set_body(std::unique_ptr<ast::CompoundStmt> declaration) {
    if (declaration == nullptr) {
        DLOG(WARNING) << "function body is nullptr";
        return false;
    }
    DVLOG(AST_LOG_LEVEL) << "set function body for function [" << DeclName()
                         << "]";
    _function_body = std::move(declaration);
    return true;
}

void FuncDeclStmt::PrintDetail(std::ostream& out,
                               const std::string& ident) const {
    // print its arguments
    out << " (";
    for (const auto& arg : _function_param) {
        out << arg->declType()->Dump();

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
    DVLOG(AST_LOG_LEVEL) << "Add argument " << param_var_decl->DeclName() << "("
                         << param_var_decl->declType()->Dump()
                         << ") to function node [" << DeclName() << "]";
    _function_param.push_back(std::move(param_var_decl));
    return true;
}

ArgumentList FuncDeclStmt::getArguments() {
    ArgumentList result;
    for (const auto& arg : _function_param) {
        result.emplace_back(
            std::make_tuple(arg->DeclName(), arg->declType(), arg->Location()));
    }
    return result;
}

bool FuncDeclStmt::IsFuncDecl() const { return true; }

bool FuncDeclStmt::HasBody() const { return _function_body != nullptr; }

Status FuncDeclStmt::visit(Visitor& visitor) { return visitor.visit(this); }

std::unique_ptr<ast::CompoundStmt>& FuncDeclStmt::Body() {
    return _function_body;
}
std::list<std::unique_ptr<ParamVarDecl>>& FuncDeclStmt::GetParams() {
    return _function_param;
}
}  // namespace hzcc::ast
