//
// Created by chen_ on 2022/3/29.
//
#include "relational.h"
#include "lexical/Token.h"
#include "AST/type/type.h"

namespace Mycc::AST {

std::string AST::RelationalExpr::GetNodeName() const {
    return "RelationalExpr";
}
RelationalExpr::RelationalExpr(const Lexical::Token& type,
                               std::unique_ptr<ASTNode> lhs,
                               std::unique_ptr<ASTNode> rhs)
    : OperatorBase(type.Location(), std::move(rhs), std::move(lhs)) {
    auto type_str = type.Value();
    if (type_str == "==") {
        this->_type = RelationalType::EQ;
    } else if (type_str == "!=") {
        this->_type = RelationalType::NE;
    } else if (type_str == "<") {
        this->_type = RelationalType::LT;
    } else if (type_str == "<=") {
        this->_type = RelationalType::LE;
    } else if (type_str == ">") {
        this->_type = RelationalType::GT;
    } else if (type_str == ">=") {
        this->_type = RelationalType::GE;
    } else {
        assert(false);
    }
}
std::string RelationalExpr::PrintAdditionalInfo(std::string_view ident) const {
    std::string ret;

    // lhs type
    ret += " " + GetLHS()->GetType()->GetName();

    // symbol type
    switch (_type) {
        case RelationalType::EQ:
            ret += " == ";
            break;
        case RelationalType::NE:
            ret += " != ";
            break;
        case RelationalType::LT:
            ret += " < ";
            break;
        case RelationalType::LE:
            ret += " <= ";
            break;
        case RelationalType::GT:
            ret += " > ";
            break;
        case RelationalType::GE:
            ret += " >= ";
            break;
        default:
            assert(false);
    }

    // dump rhs
    ret += "\n" + GetRHS()->Dump(std::string(ident) + " |") + "\n";
    ret += GetLHS()->Dump(std::string(ident) + " `");

    return ret;
}
std::shared_ptr<Type> RelationalExpr::GetType() const {
    return std::make_shared<Type>("char");
}
}  // namespace Mycc::AST