//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"
namespace hzcc::ast {
ParamVarDecl::ParamVarDecl(const Position& loc, std::string_view decl_name,
                           std::unique_ptr<TypeProxyExpr> type)  // NO_LINT
    : IDeclStmt("ParamVarDecl",decl_name, loc),
      _type(std::move(type)){
          /** ############################################################
           *  ### Runtime Assertion                                   ###
           *  ###########################################################*/
          HZCC_RUNTIME_CHECK_BLOCK({
              INTERNAL_LOG_IF(FATAL, _type != nullptr)
                  << UniqueName() << "type is nullptr";
          })};

Status hzcc::ast::ParamVarDecl::visit(Visitor& visitor) {
    return visitor.visit(this);
}

}  // namespace hzcc::ast
