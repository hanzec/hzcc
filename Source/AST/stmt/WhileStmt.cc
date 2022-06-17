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
        << HZCC_AST_PRINT_NODE_INFO("body is nullptr", this);
    HZCC_RUNTIME_CHECK(_cond != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("condition is nullptr", this);
}

const char* Hzcc::AST::WhileStmt::NodeName() const { return "WhileStmt"; }

Status WhileStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& WhileStmt::CondStmt() { return _cond; }
std::unique_ptr<ASTNode>& WhileStmt::BodyStmt() { return _body; }
void WhileStmt::PrintDetail(std::ostream& out, const std::string& ident) const {
    // print condition
    _cond->Dump(out, ident + " |");

    // print body
    _body->Dump(out, ident + " `");
}
}  // namespace Hzcc::AST
