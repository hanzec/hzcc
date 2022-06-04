//
// Created by chen_ on 2022/3/29.
//
#include "DoStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
DoStatement::DoStatement(std::unique_ptr<ASTNode> cond,   // NOLINT
                         std::unique_ptr<ASTNode> body,   // NOLINT
                         const std::pair<int, int>& pos)  // NOLINT
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
std::string DoStatement::PrintDetail(const std::string& ident) const {
    std::stringstream ss;

    // print condition
    ss << std::endl << _cond->Dump(ident + " |");

    // print body
    ss << std::endl << _body->Dump(ident + " `");
    return ss.str();
}
}  // namespace Hzcc::AST
