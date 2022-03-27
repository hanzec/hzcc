//
// Created by chen_ on 2022/3/29.
//
#include "logical.h"

#include "lexical/Token.h"
namespace Mycc::AST {

std::string AST::LogicalExpr::GetNodeName() const { return "LogicalExpr"; }
LogicalExpr::LogicalExpr(const Lexical::Token& type,
                         std::unique_ptr<ASTNode> lhs,
                         std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(lhs), std::move(rhs)) {
    switch (type.Value()[0]) {
        case '&':
            this->_type = LogicalType::kAnd;
            break;
        case '|':
            this->_type = LogicalType::kOr;
            break;
        case '=':
            this->_type = LogicalType::kEqual;
            break;
        case '!':
            this->_type = LogicalType::kNotEqual;
            break;
        default:
            assert(false);
    }
}
std::shared_ptr<Type> LogicalExpr::GetType() const {
    return std::make_shared<Type>("char");
};
}  // namespace Mycc::AST