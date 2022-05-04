//
// Created by chen_ on 2022/3/29.
//
#include "bitwise.h"

#include <glog/logging.h>

#include <cassert>

#include "lexical/Token.h"
namespace Hzcc::AST {

const char* AST::BitwiseExpr::GetNodeName() const { return "BitwiseExpr"; }
BitwiseExpr::BitwiseExpr(const Lexical::Token& type,
                         std::unique_ptr<ASTNode> lhs,
                         std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(rhs), std::move(lhs)) {
    switch (type.Value()[0]) {
        case '&':
            this->_type = BitwiseType::kAND;
            break;
        case '|':
            this->_type = BitwiseType::kOR;
            break;
        case '^':
            this->_type = BitwiseType::kXOR;
            break;
        case '<':
            this->_type = BitwiseType::kLSHIFT;
            break;
        case '>':
            this->_type = BitwiseType::kRSHIFT;
            break;
        default:
            DLOG(FATAL) << "Unknown bitwise operator";
    }
}
Status BitwiseExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
