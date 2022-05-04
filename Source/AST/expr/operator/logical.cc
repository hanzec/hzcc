//
// Created by chen_ on 2022/3/29.
//
#include "logical.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Hzcc::AST {

const char* AST::LogicalExpr::GetNodeName() const { return "LogicalExpr"; }
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
            DLOG(FATAL) << "Invalid logical operator: " << type.Value();
    }
}

std::shared_ptr<Type> LogicalExpr::GetType() const {
    return Type::GetBasicType("char", {});
}
Status LogicalExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); };

}  // namespace Hzcc::AST
