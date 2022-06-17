//
// Created by chen_ on 2022/3/28.
//
#include "DeclStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
DeclStmt::DeclStmt(std::shared_ptr<Type> type,    // NOLINT
                   const std::string_view& name,  // NOLINT
                   const Position& loc)           // NOLINT
    : ASTNode(loc), _decl_name(std::string(name)), _type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_type != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("type is nullptr", this);
    HZCC_RUNTIME_CHECK(!_decl_name.empty())
        << HZCC_AST_PRINT_NODE_INFO("decl name is empty", this);
}
bool DeclStmt::HasInitExpr() const { return false; }
bool DeclStmt::IsDeclNode() const { return true; }
std::string DeclStmt::GetName() const { return _decl_name; }
bool DeclStmt::IsFuncDecl() const { return false; }
const char* DeclStmt::NodeName() const { return "DeclStmt"; }
std::shared_ptr<Type> DeclStmt::RetType() const { return _type; }

}  // namespace Hzcc::AST
