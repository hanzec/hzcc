//
// Created by chen_ on 2022/3/26.
//
#include <glog/logging.h>
#include <stdint.h>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

#include "Expr.h"
#include "ast/DeduceValue.h"
#include "ast/type/Type.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "enums.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc::ast {

static constexpr const char* GetNodeName(LiteralType type) {
    switch (type) {
        case LiteralType::Char:
            return "CharLiteral";
        case LiteralType::Real_number:
            return "RealNumberLiteral";
        case LiteralType::String:
            return "StringLiteral";
        case LiteralType::Integer:
            return "IntegerLiteral";
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return "";
}

LiteralExpr::LiteralExpr(int64_t value, const Position& location)
    : Expr(location, GetNodeName(LiteralType::Integer)),
      _type(LiteralType::Integer),
      _value(std::to_string(value)) {}

LiteralExpr::LiteralExpr(LiteralType type,        // NO_LINT
                         const Position& loc,     // NO_LINT
                         std::string_view value)  // NO_LINT
    : Expr(loc, GetNodeName(type)),
      _type(type),
      _value(std::string(value) +
             (type == LiteralType::String ? "\\x00" : "")) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, !value.empty())
        << UniqueName() << "value is empty string";
#endif
}

std::optional<DeduceValue> LiteralExpr::GetDeducedValue() const {
    switch (_type) {
        case LiteralType::Char:
            return DeduceValue(_value[0]);
        case LiteralType::Real_number:
            return DeduceValue(std::stod(_value));
        case LiteralType::String:
            return std::nullopt;
        case LiteralType::Integer:
            return DeduceValue(static_cast<int64_t>(std::stoi(_value)));
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return std::nullopt;
}

std::shared_ptr<Type> LiteralExpr::type() const {
    switch (_type) {
        case LiteralType::Char:
            return GetNumericalTypeOf<PrimitiveType::kChar>();
        case LiteralType::Real_number:
            return GetNumericalTypeOf<PrimitiveType::kFloat>();
        case LiteralType::String:
            return std::make_shared<ArrayType>(
                GetNumericalTypeOf<PrimitiveType::kChar>(),
                std::make_unique<LiteralExpr>(_value.size() + 1, loc()));
        case LiteralType::Integer:
            return GetNumericalTypeOf<PrimitiveType::kInt>();
        default:
            DLOG_ASSERT(false) << "unexpected literal type";
    }
    return nullptr;
}
}  // namespace hzcc::ast
