
//
// Created by chen_ on 2022/3/29.
//
#include "if.h"
namespace Hzcc::AST {
const char *IfStatement::NodeName() const { return "IfStatement"; }

Status IfStatement::visit(ASTVisitor &visitor) { return visitor.visit(this); }
bool IfStatement::setElse(std::unique_ptr<ASTNode> Else) {
    if ((_else_statement_ = std::move(Else)) == nullptr)
        return false;
    else
        return true;
}

bool IfStatement::addElseIf(std::unique_ptr<ASTNode> Cond,
                            std::unique_ptr<ASTNode> Body) {
    if (Cond == nullptr || Body == nullptr)
        return false;
    else {
        _elseIfs.emplace_back(std::move(Cond), std::move(Body));
        return true;
    }
}
bool IfStatement::HasElse() const { return _else_statement_ != nullptr; }
bool IfStatement::HasBody() const { return true; }
std::unique_ptr<ASTNode> &IfStatement::CondStmt() { return _condition; }
std::unique_ptr<ASTNode> &IfStatement::BodyStmt() { return _if_body_statement; }
std::vector<std::pair<std::unique_ptr<ASTNode>, std::unique_ptr<ASTNode>>>
    &IfStatement::ElseIfStmt() {
    return _elseIfs;
}
std::string IfStatement::PrintAdditionalInfo(const std::string &ident) const {
    std::stringstream ss;

    // print condition
    ss << std::endl << _condition->Dump(ident + " |");

    // print body
    ss << std::endl
       << _if_body_statement->Dump(
              ident +
              (_elseIfs.empty() && _else_statement_ == nullptr ? " `" : " |"));

    // print else if
    for (auto &else_if : _elseIfs) {
        ss << std::endl
           << else_if.first->Dump(ident + (else_if == _elseIfs.back() &&
                                                   _else_statement_ == nullptr
                                               ? " `"
                                               : " |"));
    }

    // print else
    if (_else_statement_ != nullptr) {
        ss << std::endl << _else_statement_->Dump(ident + " `");
    }

    return ss.str();
}
std::unique_ptr<ASTNode> &IfStatement::ElseStmt() { return _else_statement_; }

#ifdef NDEBUG
std::string IfStatement::Dump(const std::string &ident) const {
    std::string ret;

    // print parameter
    ret += _if_body_statement->Dump(ident) + "\n";

    // dump else
    for (const auto &stmt : _elseIfs) {
        ret += stmt.second->Dump(ident) + "\n";
    }

    if (_else_statement_) {
        ret += _else_statement_->Dump(ident) + "\n";
    }

    return ret;
}
#endif

}  // namespace Hzcc::AST
