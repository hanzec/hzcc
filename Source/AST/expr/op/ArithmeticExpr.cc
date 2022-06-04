//
// Created by Hanze Chen on 2022/3/29.
//
#include "ArithmeticExpr.h"

#include <algorithm>

#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
constexpr static std::array<const char*,
                            ArithmeticType::kArithmeticType_ENUM_SIZE>
    kArithmeticStr = {"add", "sub", "mul", "div", "mod"};

ArithmeticExpr::ArithmeticExpr(const std::string_view& type,         // NOLINT
                               std::unique_ptr<ASTNode> lhs,         // NOLINT
                               std::unique_ptr<ASTNode> rhs,         // NOLINT
                               const std::pair<int, int>& location)  // NOLINT
    : OperatorBase(std::move(lhs), std::move(rhs), location) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!type.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type string empty", this);
    HZCC_RUNTIME_CHECK(type.length() == 1)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type len mismatch", this);

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '+':
            this->_type = ArithmeticType::kArithmeticType_Add;
            break;
        case '-':
            this->_type = ArithmeticType::kArithmeticType_Sub;
            break;
        case '*':
            this->_type = ArithmeticType::kArithmeticType_Mul;
            break;
        case '/':
            this->_type = ArithmeticType::kArithmeticType_Div;
            break;
        case '%':
            this->_type = ArithmeticType::kArithmeticType_Mod;
            break;
        default:
            HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_CHECK_ERROR_INFO(
                "type: [" + std::string(type) + "] not supported", this);
    }
}

const char* ArithmeticExpr::NodeName() const { return "ArithmeticExpr"; }

std::optional<DeduceValue> ArithmeticExpr::GetDeducedValue() const {
    if (!(GetLHS()->GetDeducedValue().has_value() &&
          GetRHS()->GetDeducedValue().has_value())) {
        return std::nullopt;
    }

    switch (this->_type) {
        case ArithmeticType::kArithmeticType_Add:
            return GetLHS()->GetDeducedValue().value() +
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Sub:
            return GetLHS()->GetDeducedValue().value() -
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Mul:
            return GetLHS()->GetDeducedValue().value() *
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Div:
            return GetLHS()->GetDeducedValue().value() /
                   GetRHS()->GetDeducedValue().value();
        case ArithmeticType::kArithmeticType_Mod:
            return GetLHS()->GetDeducedValue().value() %
                   GetRHS()->GetDeducedValue().value();
        default:
            return std::nullopt;
    }
}

Status ArithmeticExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}

ArithmeticType ArithmeticExpr::GetOpType() const { return _type; }

std::string ArithmeticExpr::PrintDetail(const std::string& ident) const {
    std::stringstream ss;

    // print node info
    ss << kArithmeticStr[_type] << ' ' << GetLHS()->RetType()->GetName();

    // print LHS and RHS info
    std::string new_ident(ident);
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    ss << std::endl << GetLHS()->Dump(new_ident + " |");
    ss << std::endl << GetRHS()->Dump(new_ident + " `");

    return ss.str();
};

}  // namespace Hzcc::AST