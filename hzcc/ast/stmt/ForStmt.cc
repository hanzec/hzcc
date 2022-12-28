//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"
namespace hzcc::ast {
ForStmt::ForStmt(const Position& location, std::unique_ptr<Stmt> init,
                 std::unique_ptr<Stmt> cond, std::unique_ptr<Stmt> step,
                 std::unique_ptr<Stmt> body)  // NOLINT
    : Stmt(location, "ForStmt"),
      _init(std::move(init)),
      _cond(std::move(cond)),
      _step(std::move(step)),
      _body(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _body != nullptr)
        << UniqueName() << "body is nullptr";
    INTERNAL_LOG_IF(FATAL, _step != nullptr)
        << UniqueName() << "step is nullptr";
    INTERNAL_LOG_IF(FATAL, _init != nullptr)
        << UniqueName() << "init is nullptr";
    INTERNAL_LOG_IF(FATAL, _cond != nullptr)
        << UniqueName() << "condition is nullptr";
#endif
}

Status ForStmt::visit(Visitor& visitor) { return visitor.visit(this); }

const std::unique_ptr<Stmt>& ForStmt::InitStmt() { return _init; }
const std::unique_ptr<Stmt>& ForStmt::CondStmt() { return _cond; }
const std::unique_ptr<Stmt>& ForStmt::StepStmt() { return _step; }
const std::unique_ptr<Stmt>& ForStmt::BodyStmt() { return _body; }
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
}  // namespace hzcc::ast
