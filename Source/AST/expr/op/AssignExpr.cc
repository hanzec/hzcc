//
// Created by Hanze Chen on 2022/3/29.
//
#include "AssignExpr.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
AssignExpr::AssignExpr(const Position& location, const std::string_view& type,
                       std::unique_ptr<ASTNode> lhs,
                       std::unique_ptr<ASTNode> rhs)  // NO_LINT
    : OperatorBase(location, std::move(lhs), std::move(rhs)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!type.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type string empty", this);
    HZCC_RUNTIME_CHECK(
        (type.length() == 2 && type[0] != '=') ||
        (type.length() == 1 && type[0] == '=') ||
        (type.length() == 3 && (type[0] == '<' || type[0] == '>')))
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("type len mismatch", this);

    /** #####################################################################
     *  ### Class initialization                                          ###
     *  ##################################################################### */
    switch (type[0]) {
        case '=':
            this->_type = AssignType::kAssignType_Assign;
            break;
        case '+':
            this->_type = AssignType::kAssignType_AddAssign;
            break;
        case '-':
            this->_type = AssignType::kAssignType_SubAssign;
            break;
        case '*':
            this->_type = AssignType::kAssignType_MulAssign;
            break;
        case '/':
            this->_type = AssignType::kAssignType_DivAssign;
            break;
        case '%':
            this->_type = AssignType::kAssignType_ModAssign;
            break;
        case '<':
            this->_type = AssignType::kAssignType_LShiftAssign;
            break;
        case '>':
            this->_type = AssignType::kAssignType_RShiftAssign;
            break;
        case '&':
            this->_type = AssignType::kAssignType_AndAssign;
            break;
        case '|':
            this->_type = AssignType::kAssignType_OrAssign;
            break;
        case '^':
            this->_type = AssignType::kAssignType_XorAssign;
            break;
        default:
            HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_CHECK_ERROR_INFO(
                "type: [" + std::string(type) + "] not supported", this);
    }
};

const char* AST::AssignExpr::NodeName() const { return "AssignExpr"; }

void AssignExpr::PrintDetail(std::ostream& out,
                             const std::string& ident) const {
    out << GetLHS()->RetType()->GetName();

    // print symbol
    switch (_type) {
        case kAssignType_Assign:
            out << " = ";
            break;
        case kAssignType_AddAssign:
            out << " += ";
            break;
        case kAssignType_SubAssign:
            out << " -= ";
            break;
        case kAssignType_MulAssign:
            out << " *= ";
            break;
        case kAssignType_DivAssign:
            out << " /= ";
            break;
        case kAssignType_ModAssign:
            out << " %= ";
            break;
        case kAssignType_LShiftAssign:
            out << " <<= ";
            break;
        case kAssignType_RShiftAssign:
            out << " >>= ";
            break;
        case kAssignType_AndAssign:
            out << " &= ";
            break;
        case kAssignType_OrAssign:
            out << " |= ";
            break;
        case kAssignType_XorAssign:
            out << " ^= ";
            break;
        default:
            DLOG(FATAL) << "Unknown assign op: " << _type;
    }

    // print LHS and RHS
    GetLHS()->Dump(out, ident + " |");
    GetRHS()->Dump(out, ident + " `");
}
Status AssignExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
AssignType AssignExpr::GetAssignType() const { return _type; }

}  // namespace Hzcc::AST
