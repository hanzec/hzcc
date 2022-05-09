//
// Created by chen_ on 2022/3/29.
//
#include "while.h"
namespace Hzcc::AST {
const char* Hzcc::AST::WhileStatement::NodeName() const {
    return "WhileStatement";
}

Status WhileStatement::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
std::unique_ptr<ASTNode>& WhileStatement::CondStmt() { return _cond; }
std::unique_ptr<ASTNode>& WhileStatement::BodyStmt() { return _body; }
std::string WhileStatement::PrintAdditionalInfo(
    const std::string& ident) const {
    std::stringstream ss;

    // print condition
    ss << std::endl << _cond->Dump(ident + " |");

    // print body
    ss << std::endl << _body->Dump(ident + " `");
    return ss.str();
}
WhileStatement::WhileStatement(std::unique_ptr<ASTNode> cond,
                               std::unique_ptr<ASTNode> body,
                               std::pair<int, int> location)
    : ASTNode(location), _cond(std::move(cond)), _body(std::move(body)) {
    DLOG_ASSERT(_cond != nullptr)
        << " condition is nullptr for while statement (id: " << GetNodeId()
        << ")";
    DLOG_ASSERT(_body != nullptr)
        << " body is nullptr for while statement (id: " << GetNodeId() << ")";
}

#ifdef NDEBUG
std::string WhileStatement::Dump(const std::string& ident) const {
    std::string ret;

    // print parameter
    ret += _body->Dump(ident) + "\n";
    return ret;
}
#endif
}  // namespace Hzcc::AST
