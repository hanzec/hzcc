//
// Created by chen_ on 2022/3/29.
//
#include "WhileStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
WhileStmt::WhileStmt(const Position& loc, std::unique_ptr<ASTNode> cond,
                     std::unique_ptr<ASTNode> body)  // NO_LINT
    : ASTNode(loc), _cond(std::move(cond)), _body(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_body != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("body is nullptr", this);
    HZCC_RUNTIME_CHECK(_cond != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("condition is nullptr", this);
}

const char* Hzcc::AST::WhileStmt::NodeName() const { return "WhileStmt"; }

Status WhileStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& WhileStmt::CondStmt() { return _cond; }
std::unique_ptr<ASTNode>& WhileStmt::BodyStmt() { return _body; }
std::string WhileStmt::PrintDetail(const std::string& ident) const {
    std::stringstream ss;

    // print condition
    ss << std::endl << _cond->Dump(ident + " |");

    // print body
    ss << std::endl << _body->Dump(ident + " `");
    return ss.str();
}
}  // namespace Hzcc::AST
