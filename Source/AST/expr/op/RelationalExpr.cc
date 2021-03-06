//
// Created by Hanze Chen on 2022/3/29.
//
#include "RelationalExpr.h"

#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
RelationalExpr::RelationalExpr(const Position& loc,
                               const std::string_view& type,
                               std::unique_ptr<ASTNode> lhs,
                               std::unique_ptr<ASTNode> rhs)  // NOLINT
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
    switch (type[0]) {
        case '=':
            this->_type = RelationalType::kRelationalType_Equal;
            break;
        case '!':
            this->_type = RelationalType::kRelationalType_NEqual;
            break;
        case '<':
            if (type[1] == '=')
                this->_type = RelationalType::kRelationalType_LessEqual;
            else
                this->_type = RelationalType::kRelationalType_Less;
            break;
        case '>':
            if (type[1] == '=')
                this->_type = RelationalType::kRelationalType_GreaterEqual;
            else
                this->_type = RelationalType::kRelationalType_Greater;
            break;
        default:
            HZCC_RUNTIME_CHECK(false) << HZCC_AST_PRINT_CHECK_ERROR_INFO(
                "type: [" + std::string(type) + "] not supported", this);
    }
}

const char* AST::RelationalExpr::NodeName() const { return "RelationalExpr"; }

void RelationalExpr::PrintDetail(std::ostream& out,
                                 const std::string& ident) const {
    // lhs type
    out << " " + GetLHS()->RetType()->GetName();

    // symbol type
    switch (_type) {
        case RelationalType::kRelationalType_Equal:
            out << " == ";
            break;
        case RelationalType::kRelationalType_NEqual:
            out << " != ";
            break;
        case RelationalType::kRelationalType_Less:
            out << " < ";
            break;
        case RelationalType::kRelationalType_LessEqual:
            out << " <= ";
            break;
        case RelationalType::kRelationalType_Greater:
            out << " > ";
            break;
        case RelationalType::kRelationalType_GreaterEqual:
            out << " >= ";
            break;
        default:
            DLOG(FATAL) << "Unknown relational op: " << _type;
    }

    // dump rhs
    GetLHS()->Dump(out, ident + " |");
    GetRHS()->Dump(out, ident + " `");
}

std::shared_ptr<Type> RelationalExpr::RetType() const {
    return Type::GetTypeOf("char", {});
}
Status RelationalExpr::visit(ASTVisitor& visitor) {
    return visitor.visit(this);
}
RelationalType RelationalExpr::OpType() const { return _type; }
}  // namespace Hzcc::AST
