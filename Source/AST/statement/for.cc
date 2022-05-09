//
// Created by chen_ on 2022/3/29.
//
#include "for.h"
namespace Hzcc::AST {
const char* ForStatement::NodeName() const { return "ForStatement"; }

Status ForStatement::visit(ASTVisitor& visitor) { return visitor.visit(this); }

std::unique_ptr<ASTNode>& ForStatement::InitStmt() { return _init; }
std::unique_ptr<ASTNode>& ForStatement::CondStmt() { return _cond; }
std::unique_ptr<ASTNode>& ForStatement::StepStmt() { return _step; }
std::unique_ptr<ASTNode>& ForStatement::BodyStmt() { return _body; }
std::string ForStatement::PrintAdditionalInfo(const std::string& ident) const {
    std::stringstream ss;

    // print init
    ss << std::endl << _init->Dump(ident + " |");

    // print condition
    ss << std::endl << _cond->Dump(ident + " |");

    // print step
    ss << std::endl << _step->Dump(ident + " |");

    // print body
    ss << std::endl << _body->Dump(ident + " `");
    return ss.str();
}

ForStatement::ForStatement(std::unique_ptr<ASTNode> init,
                           std::unique_ptr<ASTNode> cond,
                           std::unique_ptr<ASTNode> step,
                           std::unique_ptr<ASTNode> body,
                           std::pair<int, int> location)
    : ASTNode(location),
      _init(std::move(init)),
      _cond(std::move(cond)),
      _step(std::move(step)),
      _body(std::move(body)) {
    DLOG_ASSERT(_init != nullptr)
        << " init is nullptr for for statement (id = " << GetNodeId() << ")";
    DLOG_ASSERT(_cond != nullptr)
        << " cond is nullptr for for statement (id = " << GetNodeId() << ")";
    DLOG_ASSERT(_step != nullptr)
        << " step is nullptr for for statement (id = " << GetNodeId() << ")";
    DLOG_ASSERT(_body != nullptr)
        << " body is nullptr for for statement (id = " << GetNodeId() << ")";
}

#ifdef NDEBUG
std::string ForStatement::Dump(const std::string& ident) const {
    std::string ret;

    // print parameter
    ret += _body->Dump(ident) + "\n";
    return ret;
}
#endif
}  // namespace Hzcc::AST
