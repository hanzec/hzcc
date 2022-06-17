//
// Created by Hanze Chen on 2022/3/29.
//
#include "BitwiseExpr.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
BitwiseExpr::BitwiseExpr(const Position& loc, const std::string_view& type,
                         std::unique_ptr<ASTNode> lhs,
                         std::unique_ptr<ASTNode> rhs)  // NOLINT
    : OperatorBase(loc, std::move(rhs), std::move(lhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!type.empty())
        << HZCC_AST_PRINT_NODE_INFO("type string empty", this);
    HZCC_RUNTIME_CHECK(
        type.length() == 1 ||
        (type.length() == 2 && (type[0] == '<' || type[0] == '>')))
        << HZCC_AST_PRINT_NODE_INFO("type len mismatch", this);

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '&':
            this->_type = BitwiseType::kBitwiseType_AND;
            break;
        case '|':
            this->_type = BitwiseType::kBitwiseType_OR;
            break;
        case '^':
            this->_type = BitwiseType::kBitwiseType_XOR;
            break;
        case '<':
            this->_type = BitwiseType::kBitwiseType_LSHIFT;
            break;
        case '>':
            this->_type = BitwiseType::kBitwiseType_RSHIFT;
            break;
        default:
            HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_NODE_INFO(
                "type: [" + std::string(type) + "] not supported", this);
    }
}

const char* AST::BitwiseExpr::NodeName() const { return "BitwiseExpr"; }

Status BitwiseExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
