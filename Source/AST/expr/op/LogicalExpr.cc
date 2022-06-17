//
// Created by Hanze Chen on 2022/3/29.
//
#include "LogicalExpr.h"

#include "AST/type/BuiltinType.h"
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
        << HZCC_AST_PRINT_NODE_INFO("type string empty", this);
    HZCC_RUNTIME_CHECK(type.length() == 2)
        << HZCC_AST_PRINT_NODE_INFO("type len mismatch", this);

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    if (type[0] == '&') {
        this->_type = kLogicalType_And;
    } else if (type[0] == '|') {
        this->_type = kLogicalType_Or;
    } else {
        HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_NODE_INFO(
            "type: [" + std::string(type) + "] not supported", this);
    }
}

const char* AST::LogicalExpr::NodeName() const { return "LogicalExpr"; }

std::shared_ptr<Type> LogicalExpr::RetType() const {
    const static auto ret_type =
        std::make_shared<BuiltinType>(kPrimitiveType_char);
    return ret_type;
}

Status LogicalExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }

LogicalType LogicalExpr::GetLogicalType() const { return _type; }

void LogicalExpr::PrintDetail(std::ostream& out,
                              const std::string& ident) const {
    // print logical type
    out << (_type == kLogicalType_And ? "&&" : "||");

    // print lhs
    GetLHS()->Dump(out, ident + " |");

    // print rhs
    GetLHS()->Dump(out, ident + " `");
};

}  // namespace Hzcc::AST
