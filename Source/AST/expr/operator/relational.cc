//
// Created by chen_ on 2022/3/29.
//
#include "relational.h"

#include "AST/type/Type.h"
#include "lexical/Token.h"

namespace Hzcc::AST {

const char* AST::RelationalExpr::NodeName() const { return "RelationalExpr"; }
RelationalExpr::RelationalExpr(const Lexical::Token& type,
                               std::unique_ptr<ASTNode> lhs,
                               std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(lhs), std::move(rhs)) {
    auto type_str = type.Value();
    if (type_str == "==") {
        this->_type = RelationalType::kRelationalType_Equal;
    } else if (type_str == "!=") {
        this->_type = RelationalType::kRelationalType_NEqual;
    } else if (type_str == "<") {
        this->_type = RelationalType::kRelationalType_Less;
    } else if (type_str == "<=") {
        this->_type = RelationalType::kRelationalType_LessEqual;
    } else if (type_str == ">") {
        this->_type = RelationalType::kRelationalType_Greater;
    } else if (type_str == ">=") {
        this->_type = RelationalType::kRelationalType_GreaterEqual;
    } else {
        DLOG(FATAL) << "Unknown relational operator: " << type_str;
    }
}
std::string RelationalExpr::PrintAdditionalInfo(
    const std::string& ident) const {
    std::stringstream ret;

    // lhs type
    ret << " " + GetLHS()->GetType()->GetName();

    // symbol type
    switch (_type) {
        case RelationalType::kRelationalType_Equal:
            ret << " == ";
            break;
        case RelationalType::kRelationalType_NEqual:
            ret << " != ";
            break;
        case RelationalType::kRelationalType_Less:
            ret << " < ";
            break;
        case RelationalType::kRelationalType_LessEqual:
            ret << " <= ";
            break;
        case RelationalType::kRelationalType_Greater:
            ret << " > ";
            break;
        case RelationalType::kRelationalType_GreaterEqual:
            ret << " >= ";
            break;
        default:
            DLOG(FATAL) << "Unknown relational operator: " << _type;
    }

    // dump rhs
    ret << "\n" + GetLHS()->Dump(ident + " |") + "\n" +
               GetRHS()->Dump(ident + " `");

    return ret.str();
}
std::shared_ptr<Type> RelationalExpr::GetType() const {
    return Type::GetBasicType("char", {});
}
Status RelationalExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
RelationalType RelationalExpr::OpType() const { return _type; }
}  // namespace Hzcc::AST
