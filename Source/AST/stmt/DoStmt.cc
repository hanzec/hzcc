//
// Created by chen_ on 2022/3/29.
//
#include "DoStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
DoStatement::DoStatement(std::unique_ptr<ASTNode> cond,  // NOLINT
                         std::unique_ptr<ASTNode> body,  // NOLINT
                         const Position& pos)            // NOLINT
    : ASTNode(pos), _cond(std::move(cond)), _body(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_body != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("body is nullptr", this);
    HZCC_RUNTIME_CHECK(_cond != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("condition is nullptr", this);
}

const char* AST::DoStatement::NodeName() const { return "DoStatement"; }
Status DoStatement::visit(ASTVisitor& visitor) { return visitor.visit(this); }
std::unique_ptr<ASTNode>& DoStatement::CondStmt() { return _cond; }
std::unique_ptr<ASTNode>& DoStatement::BodyStmt() { return _body; }
void DoStatement::PrintDetail(std::ostream& out,
                              const std::string& ident) const {
    _cond->Dump(out, ident + " |");
    _body->Dump(out, ident + " `");
}
}  // namespace Hzcc::AST
