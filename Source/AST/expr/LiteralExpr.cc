//
// Created by chen_ on 2022/3/26.
//
#include "LiteralExpr.h"

#include "AST/DeduceValue.h"
#include "AST/type/ArrayType.h"
#include "AST/type/Type.h"
#include "AST/utils/macro.h"
namespace Hzcc::AST {
LiteralExpr::LiteralExpr(int64_t value, const Position& location)
    : ASTNode(location),
      _type(kLiteralType_Integer),
      _value(std::to_string(value)) {}

LiteralExpr::LiteralExpr(LiteralType type,                     // NO_LINT
                         const std::string_view& value,        // NO_LINT
                         const std::pair<int, int>& location)  // NO_LINT
    : ASTNode(location),
      _type(type),
      _value(std::string(value) +
             (type == LiteralType::kLiteralType_String ? "\\x00" : "")) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(!value.empty())
        << HZCC_AST_PRINT_NODE_INFO("value is empty string", this);
}

const char* LiteralExpr::NodeName() const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return "CharLiteral";
        case LiteralType::kLiteralType_Real_number:
            return "RealNumberLiteral";
        case LiteralType::kLiteralType_String:
            return "StringLiteral";
        case LiteralType::kLiteralType_Integer:
            return "IntegerLiteral";
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return "";
}

std::optional<DeduceValue> LiteralExpr::GetDeducedValue() const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return DeduceValue(_value[0]);
        case LiteralType::kLiteralType_Real_number:
            return DeduceValue(std::stod(_value));
        case LiteralType::kLiteralType_String:
            return std::nullopt;
        case LiteralType::kLiteralType_Integer:
            return DeduceValue(static_cast<int64_t>(std::stoi(_value)));
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return std::nullopt;
}

void LiteralExpr::PrintDetail(std::ostream& out,
                              const std::string& ident) const {
    switch (_type) {
        case LiteralType::kLiteralType_Char:
            out << "char \'" << _value << "\'";
            break;
        case LiteralType::kLiteralType_Real_number:
            out << "real_number " << _value;
            break;
        case LiteralType::kLiteralType_String:
            out << "string \"" << _value << "\"";
            break;
        case LiteralType::kLiteralType_Integer:
            out << "int " << _value;
            break;
        case kLiteralType_Max:
            DLOG_ASSERT(false) << "unexpected literal type [kLiteralType_Max]";
    }
}

std::shared_ptr<Type> LiteralExpr::RetType() const {
    const static std::shared_ptr<Type> kCharArrType =
        std::make_shared<ArrayType>(
            GetCharType(),
            std::make_unique<LiteralExpr>(_value.size() + 1, Location()));

    switch (_type) {
        case LiteralType::kLiteralType_Char:
            return GetCharType();
        case LiteralType::kLiteralType_Real_number:
            return GetFloatType();
        case LiteralType::kLiteralType_String:
            return kCharArrType;
        case LiteralType::kLiteralType_Integer:
            return GetIntType();
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return nullptr;
}

bool LiteralExpr::IsReturnLValue() const { return false; }
Status LiteralExpr::visit(ASTVisitor& visitor) { return visitor.visit(this); }
LiteralType LiteralExpr::GetLiteralType() const { return _type; }
bool LiteralExpr::IsLiteral() const { return true; }
const std::string& LiteralExpr::GetValue() const { return _value; }

}  // namespace Hzcc::AST
