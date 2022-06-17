
//
// Created by chen_ on 2022/3/29.
//
#include "IfStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {

IfStmt::IfStmt(const Position &location, std::unique_ptr<ASTNode> cond,
               std::unique_ptr<ASTNode> body)  // NOLINT
    : ASTNode(location),
      _condition(std::move(cond)),
      _if_body_statement(std::move(body)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(body != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("body is nullptr", this);
    HZCC_RUNTIME_CHECK(cond != nullptr)
        << HZCC_AST_PRINT_NODE_INFO("condition is nullptr", this);
};

const char *IfStmt::NodeName() const { return "IfStmt"; }

Status IfStmt::visit(ASTVisitor &visitor) { return visitor.visit(this); }
bool IfStmt::setElse(std::unique_ptr<ASTNode> Else) {
    if ((_else_statement_ = std::move(Else)) == nullptr)
        return false;
    else
        return true;
}

bool IfStmt::addElseIf(std::unique_ptr<ASTNode> Cond,
                       std::unique_ptr<ASTNode> Body) {
    if (Cond == nullptr || Body == nullptr)
        return false;
    else {
        _elseIfs.emplace_back(std::move(Cond), std::move(Body));
        return true;
    }
}
bool IfStmt::HasElse() const { return _else_statement_ != nullptr; }
bool IfStmt::HasBody() const { return true; }
const std::unique_ptr<ASTNode> &IfStmt::CondStmt() { return _condition; }
const std::unique_ptr<ASTNode> &IfStmt::BodyStmt() {
    return _if_body_statement;
}
std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>
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
std::unique_ptr<ASTNode> &IfStmt::ElseStmt() { return _else_statement_; }

// return nullptr for return type
std::shared_ptr<Type> IfStmt::RetType() const { return nullptr; }
}  // namespace Hzcc::AST
