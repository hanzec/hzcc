//
// Created by chen_ on 2022/3/29.
//
#include "ForStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
ForStmt::ForStmt(const Position& location, std::unique_ptr<ASTNode> init,
                 std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> step,
                 std::unique_ptr<ASTNode> body)  // NOLINT
    : ASTNode(location),
      _init(std::move(init)),
      _cond(std::move(cond)),
      _step(std::move(step)),
      _body(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_body != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("body is nullptr", this);
    HZCC_RUNTIME_CHECK(_step != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("step is nullptr", this);
    HZCC_RUNTIME_CHECK(_init != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("init is nullptr", this);
    HZCC_RUNTIME_CHECK(_cond != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("condition is nullptr", this);
}

const char* ForStmt::NodeName() const { return "ForStmt"; }

Status ForStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }

const std::unique_ptr<ASTNode>& ForStmt::InitStmt() { return _init; }
const std::unique_ptr<ASTNode>& ForStmt::CondStmt() { return _cond; }
const std::unique_ptr<ASTNode>& ForStmt::StepStmt() { return _step; }
const std::unique_ptr<ASTNode>& ForStmt::BodyStmt() { return _body; }
void ForStmt::PrintDetail(std::ostream& out, const std::string& ident) const {
    // print init
    _init->Dump(out, ident + " |");

    // print condition
    _cond->Dump(out, ident + " |");

    // print step
    _step->Dump(out, ident + " |");

    // print body
    _body->Dump(out, ident + " `");
}
}  // namespace Hzcc::AST
