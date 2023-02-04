//
// Created by chen_ on 2022/3/27.
//
#include <glog/logging.h>

#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {

FuncDeclStmt::FuncDeclStmt(
    const Position& loc, std::string_view name,
    std::unique_ptr<TypeProxyExpr> return_type)  // NO_LINT
    : IDeclStmt(DeclType::FUNC, "FunctionDecl", name, loc),
      _ret_type(std::move(return_type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK({
        LOG_IF(FATAL, _ret_type != nullptr)
            << UniqueName() << "type is nullptr";
    })
}

bool FuncDeclStmt::set_body(std::unique_ptr<ast::CompoundStmt> declaration) {
    if (declaration == nullptr) {
        DLOG(WARNING) << "function body is nullptr";
        return false;
    }
    DVLOG(AST_LOG) << "set function body for function [" << name() << "]";
    _func_body = std::move(declaration);
    return true;
}

bool FuncDeclStmt::AddFunctionArgument(
    std::unique_ptr<ParamVarDecl> param_var_decl) {
    if (param_var_decl == nullptr) {
        return false;
    }
    DVLOG(AST_LOG) << "Add argument " << param_var_decl->name() << "("
                   << param_var_decl->type()->to_str() << ") to function node ["
                   << name() << "]";
    _func_param.push_back(std::move(param_var_decl));
    return true;
}

bool FuncDeclStmt::has_body() const { return _func_body != nullptr; }

Status FuncDeclStmt::visit(Visitor& visitor) { return visitor.visit(this); }

std::unique_ptr<ast::CompoundStmt>& FuncDeclStmt::body_stmt() {
    return _func_body;
}
std::list<std::unique_ptr<ParamVarDecl>>& FuncDeclStmt::params() {
    return _func_param;
}
}  // namespace hzcc::ast
