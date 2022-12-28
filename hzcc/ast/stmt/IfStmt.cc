
//
// Created by chen_ on 2022/3/29.
//
#include "ast/Stmt.h"
#include "utils/logging.h"
namespace hzcc::ast {

IfStmt::IfStmt(const Position &location, std::unique_ptr<Stmt> cond,
               std::unique_ptr<Stmt> body)  // NOLINT
    : Stmt(location, "IfStmt"),
      _condition(std::move(cond)),
      _if_body_statement(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, body != nullptr)
        << UniqueName() << "body is nullptr";
    INTERNAL_LOG_IF(FATAL, cond != nullptr)
        << UniqueName() << "condition is nullptr";
#endif
};

Status IfStmt::visit(Visitor &visitor) { return visitor.visit(this); }
bool IfStmt::set_else(std::unique_ptr<Stmt> Else) {
    return (_else_statement_ = std::move(Else)) != nullptr;
}

void IfStmt::addElseIf(std::unique_ptr<Stmt> Cond, std::unique_ptr<Stmt> Body) {
    HZCC_RUNTIME_CHECK_BLOCK({
        LOG_IF(FATAL, Cond == nullptr || Body == nullptr)
            << UniqueName() << "Cond or Body is nullptr";
    })

    _elseIfs.emplace_back(std::move(Cond), std::move(Body));
}
bool IfStmt::HasElse() const { return _else_statement_ != nullptr; }
bool IfStmt::HasBody() const { return true; }
const std::unique_ptr<Stmt> &IfStmt::CondStmt() { return _condition; }
const std::unique_ptr<Stmt> &IfStmt::BodyStmt() { return _if_body_statement; }
std::vector<std::pair<std::unique_ptr<Stmt>, std::unique_ptr<Stmt>>>
    &IfStmt::ElseIfStmt() {
    return _elseIfs;
}
void IfStmt::PrintDetail(std::ostream &out, const std::string &ident) const {
    // print condition
    _condition->Dump(out, ident + " |");

    // print body
    _if_body_statement->Dump(
        out, ident + (_elseIfs.empty() && _else_statement_ == nullptr ? " `"
                                                                      : " |"));

    // print else if
    for (auto &else_if : _elseIfs) {
        else_if.first->Dump(out, ident + (else_if == _elseIfs.back() &&
                                                  _else_statement_ == nullptr
                                              ? " `"
                                              : " |"));
    }

    // print else
    if (_else_statement_ != nullptr) {
        _else_statement_->Dump(out, ident + " `");
    }
}
std::unique_ptr<Stmt> &IfStmt::ElseStmt() { return _else_statement_; }
}  // namespace hzcc::ast
