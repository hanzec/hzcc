//
// Created by chen_ on 2022/3/29.
//
#include "StructDeclStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
StructDeclStmt::StructDeclStmt(const std::string_view& name,
                               std::shared_ptr<StructType> type,
                               const std::pair<int, int>& location)  // NO_LINT
    : DeclStmt(name, location), _type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    // name is checked in DeclStmt
    HZCC_RUNTIME_CHECK(type != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("struct type is nullptr", this);
}

const char* StructDeclStmt::NodeName() const { return "StructDeclStmt"; }

Status StructDeclStmt::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
std::shared_ptr<Type> StructDeclStmt::RetType() const { return _type; }
}  // namespace Hzcc::AST
