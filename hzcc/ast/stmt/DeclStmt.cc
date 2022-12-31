//
// Created by chen_ on 2022/12/27.
//
#include <utility>

#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
namespace hzcc::ast {
DeclStmt::DeclStmt(const Position& loc,                            // NOLINT
                   std::list<std::unique_ptr<VarDecl>> decl_list)  // NOLINT
    : IDeclStmt("DeclStmt", "NO_NAME", loc), _decl_list(std::move(decl_list)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK({
        // name and type is checked in DeclStmt
        INTERNAL_LOG_IF(FATAL, _decl_list.empty())
            << UniqueName() << "decl list is empty";
    })
}
}  // namespace hzcc::ast