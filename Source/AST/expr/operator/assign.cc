//
// Created by chen_ on 2022/3/29.
//
#include "assign.h"

#include "lexical/Token.h"
namespace Mycc::AST {

AssignExpr::AssignExpr(const Lexical::Token& type, std::unique_ptr<ASTNode> lhs,
                       std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(lhs), std::move(rhs)) {
    switch (type.Value()[0]) {
        case '=':
            this->_type = AssignType::kAssign;
            break;
        case '+':
            this->_type = AssignType::kAddAssign;
            break;
        case '-':
            this->_type = AssignType::kSubAssign;
            break;
        case '*':
            this->_type = AssignType::kMulAssign;
            break;
        case '/':
            this->_type = AssignType::kDivAssign;
            break;
        case '%':
            this->_type = AssignType::kModAssign;
            break;
        case '<':
            this->_type = AssignType::kLShiftAssign;
            break;
        case '>':
            this->_type = AssignType::kRShiftAssign;
            break;
        case '&':
            this->_type = AssignType::kAndAssign;
            break;
        case '|':
            this->_type = AssignType::kOrAssign;
            break;
        case '^':
            this->_type = AssignType::kXorAssign;
            break;
        default:
            assert(false);
    }
};

std::string AST::AssignExpr::GetNodeName() const { return "AssignExpr"; }

std::string AssignExpr::PrintAdditionalInfo(std::string_view ident) const {
    std::string string;

    // print lhs type
    string += GetLHS()->GetType()->GetName();

    // print symbol
    switch (_type) {
        case kAssign:
            string += " = ";
            break;
        case kAddAssign:
            string += " += ";
            break;
        case kSubAssign:
            string += " -= ";
            break;
        case kMulAssign:
            string += " *= ";
            break;
        case kDivAssign:
            string += " /= ";
            break;
        case kModAssign:
            string += " %= ";
            break;
        case kLShiftAssign:
            string += " <<= ";
            break;
        case kRShiftAssign:
            string += " >>= ";
            break;
        case kAndAssign:
            string += " &= ";
            break;
        case kOrAssign:
            string += " |= ";
            break;
        case kXorAssign:
            string += " ^= ";
            break;
    }

    // print LHS and RHS
    string += "\n";
    string += GetLHS()->Dump(std::string(ident) + " |") + "\n";
    string += GetRHS()->Dump(std::string(ident) + " `");
    return string;
}

}  // namespace Mycc::AST