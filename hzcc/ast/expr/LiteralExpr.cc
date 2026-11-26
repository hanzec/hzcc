//
// Created by chen_ on 2022/3/26.
//
#include <glog/logging.h>

#include <cstdint>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

#include "Expr.h"
#include "ast/DeduceValue.h"
#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "ast/visitor.h"
#include "enums.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {
static std::array<const char[32], 4> LiteralTypeString = {
    "CharLiteral", "IntegerLiteral", "RealNumberLiteral", "StringLiteral"};

LiteralExpr::LiteralExpr(int64_t value, const Position& location)
    : Expr(location, "IntegerLiteral"),
      _value(std::to_string(value)),
      _type(LiteralType::Integer) {}

LiteralExpr::LiteralExpr(LiteralType type,        // NO_LINT
                         const Position& loc,     // NO_LINT
                         std::string_view value)  // NO_LINT
    : Expr(loc, LiteralTypeString[magic_enum::enum_integer(type)]),
      _value(value),
      _type(type) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    LOG_IF(FATAL, value.empty() && type != LiteralType::String)
        << UniqueName() << "value is empty string";
#endif
}

std::optional<DeduceValue> LiteralExpr::GetDeducedValue() const {
    switch (_type) {
        case LiteralType::Char:
            return DeduceValue(_value[0]);
        case LiteralType::Real_number:
            return DeduceValue(kDeduceValueType_Real_Number, _value);
        case LiteralType::String:
            return std::nullopt;
        case LiteralType::Integer:
            return DeduceValue(kDeduceValueType_Int, _value);
        default:
            DCHECK(false) << "unexpected literal type";
    }
    return std::nullopt;
}

QualTypePtr LiteralExpr::type() const {
    switch (_type) {
        case LiteralType::Char:
            return GetNumericalTypeOf<PrimitiveType::kChar>();
        case LiteralType::Real_number:
            return GetNumericalTypeOf<PrimitiveType::kFloat>();
        case LiteralType::String:
            return std::make_shared<QualType>(std::make_shared<ArrayType>(
                std::make_unique<LiteralExpr>(_value.size() + 1, loc()),
                GetNumericalTypeOf<PrimitiveType::kChar>()));
        case LiteralType::Integer:
            return GetNumericalTypeOf<PrimitiveType::kInt>();
        default:
            DCHECK(false) << "unexpected literal type";
    }
    return nullptr;
}
}  // namespace hzcc::ast
