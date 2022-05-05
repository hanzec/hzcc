//
// Created by chen_ on 2022/3/29.
//
#include "assign.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"
namespace Hzcc::AST {

AssignExpr::AssignExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                       std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(lhs), std::move(rhs)) {
    switch (type.Value()[0]) {
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
            DLOG(FATAL) << "Unknown assign operator: " << type.Value();
    }
};

const char* AST::AssignExpr::GetNodeName() const { return "AssignExpr"; }

std::string AssignExpr::PrintAdditionalInfo(std::string_view ident) const {
    std::string string;

    // print lhs type
    string += GetLHS()->GetType()->GetName();

    // print symbol
    switch (_type) {
        case kAssignType_Assign:
            string += " = ";
            break;
        case kAssignType_AddAssign:
            string += " += ";
            break;
        case kAssignType_SubAssign:
            string += " -= ";
            break;
        case kAssignType_MulAssign:
            string += " *= ";
            break;
        case kAssignType_DivAssign:
            string += " /= ";
            break;
        case kAssignType_ModAssign:
            string += " %= ";
            break;
        case kAssignType_LShiftAssign:
            string += " <<= ";
            break;
        case kAssignType_RShiftAssign:
            string += " >>= ";
            break;
        case kAssignType_AndAssign:
            string += " &= ";
            break;
        case kAssignType_OrAssign:
            string += " |= ";
            break;
        case kAssignType_XorAssign:
            string += " ^= ";
            break;
        default:
            DLOG(FATAL) << "Unknown assign operator: " << _type;
    }

    // print LHS and RHS
    string += "\n";
    string += GetLHS()->Dump(std::string(ident) + " |") + "\n";
    string += GetRHS()->Dump(std::string(ident) + " `");
    return string;
}
Status AssignExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
AssignType AssignExpr::GetAssignType() const { return _type; }

}  // namespace Hzcc::AST
