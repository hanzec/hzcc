//
// Created by Hanze Chen on 2022/3/29.
//
#include "LogicalExpr.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
LogicalExpr::LogicalExpr(const std::string_view& type,  // NOLINT
                         std::unique_ptr<ASTNode> lhs,  // NOLINT
                         std::unique_ptr<ASTNode> rhs,  // NOLINT
                         const Position& loc)           // NOLINT
    : OperatorBase(loc, std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!type.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type string empty", this);
    HZCC_RUNTIME_CHECK(type.length() == 2)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type len mismatch", this);

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    if (type[0] == '&') {
        this->_type = kLogicalType_And;
    } else if (type[0] == '|') {
        this->_type = kLogicalType_Or;
    } else {
        HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_CHECK_ERROR_INFO(
            "type: [" + std::string(type) + "] not supported", this);
    }
}

const char* AST::LogicalExpr::NodeName() const { return "LogicalExpr"; }

std::shared_ptr<Type> LogicalExpr::RetType() const {
    return Type::GetTypeOf("char", {});
}
Status LogicalExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
LogicalType LogicalExpr::GetLogicalType() const { return _type; }

std::string LogicalExpr::PrintDetail(const std::string& ident) const {
    std::stringstream ss;

    // print logical type
    ss << (_type == kLogicalType_And ? "&&" : "||") << std::endl;

    // print lhs
    ss << GetLHS()->Dump(ident + " |") << std::endl;

    // print rhs
    ss << GetLHS()->Dump(ident + " `");
    return ss.str();
};

}  // namespace Hzcc::AST
