//
// Created by chen_ on 2022/3/28.
//
#include "DeclStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
DeclStmt::DeclStmt(const std::string_view& name,    // NOLINT
                   const std::pair<int, int>& loc)  // NOLINT
    : ASTNode(loc), _decl_name(std::string(name)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_decl_name.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("decl name is empty", this);
}
bool DeclStmt::HasInitExpr() const { return false; }
bool DeclStmt::IsDeclNode() const { return true; }
std::string DeclStmt::GetName() const { return _decl_name; }
bool DeclStmt::IsFuncDecl() const { return false; }
const char* DeclStmt::NodeName() const { return "DeclStmt"; }

}  // namespace Hzcc::AST
