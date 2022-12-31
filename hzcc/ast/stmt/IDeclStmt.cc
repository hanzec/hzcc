//
// Created by chen_ on 2022/3/28.
//
#include "ast/Stmt.h"
namespace hzcc::ast {
IDeclStmt::IDeclStmt(const char* node_name,     // NOLINT
                     std::string_view name,     // NOLINT
                     const Position& location)  // NOLINT
    : Stmt(location, node_name), _decl_name(name) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK_BLOCK({
        INTERNAL_LOG_IF(FATAL, !_decl_name.empty())
            << UniqueName() << "decl name is empty";
    })
}

bool IDeclStmt::has_init() const { return false; }
bool IDeclStmt::IsDeclNode() const { return true; }
std::string_view IDeclStmt::decl_name() const { return _decl_name; }
bool IDeclStmt::IsFuncDecl() const { return false; }

}  // namespace hzcc::ast
