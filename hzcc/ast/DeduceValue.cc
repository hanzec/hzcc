//
// Created by chen_ on 2022/3/29.
//

#include "DeduceValue.h"

#include <stdexcept>

#include "utils/integer_literal.h"

namespace hzcc::ast {

DeduceValue::DeduceValue(char value) : _value(value) {}
DeduceValue::DeduceValue(int64_t value) : _value(value) {}
DeduceValue::DeduceValue(double value) : _value(value) {}

DeduceValue::DeduceValue(DeduceValueType type, std::string str_value) {
    switch (type) {
        case kDeduceValueType_Int: {
            int64_t value;
            const auto parsed = utils::parse_integer_literal(str_value, value);
            if (parsed.conversion.ec == std::errc::result_out_of_range) {
                throw std::out_of_range("base-" + std::to_string(parsed.base) +
                                        " integer literal '" + str_value +
                                        "' is outside the signed 64-bit range");
            }
            if (parsed.conversion.ec != std::errc{}) {
                throw std::invalid_argument(
                    "invalid base-" + std::to_string(parsed.base) + " integer literal '" +
                    str_value + "' at offset " +
                    std::to_string(parsed.conversion.ptr - str_value.data()));
            }
            _value = value;
            break;
        }
        case kDeduceValueType_Char:
            if (str_value.empty()) throw std::invalid_argument("empty character literal");
            _value = str_value.front();
            break;
        case kDeduceValueType_Real_Number: {
            size_t end = 0;
            try {
                _value = std::stod(str_value, &end);
            } catch (const std::out_of_range&) {
                throw std::out_of_range("real literal '" + str_value +
                                        "' is outside the double range");
            } catch (const std::invalid_argument&) {
                throw std::invalid_argument("invalid real literal '" + str_value + "'");
            }
            if (end != str_value.size()) {
                throw std::invalid_argument("invalid real literal '" + str_value +
                                            "' at offset " + std::to_string(end));
            }
            break;
        }
        default:
            throw std::invalid_argument("unknown constant type " + std::to_string(type) +
                                        " for literal '" + str_value + "'");
    }
}

char DeduceValue::as_char() const { return std::get<char>(_value); }

int64_t DeduceValue::AsInt() const {
    return std::visit([](auto value) { return static_cast<int64_t>(value); }, _value);
}

double DeduceValue::AsReal() const {
    return std::visit([](auto value) { return static_cast<double>(value); }, _value);
}

DeduceValueType DeduceValue::GetType() const {
    if (std::holds_alternative<int64_t>(_value)) return kDeduceValueType_Int;
    if (std::holds_alternative<char>(_value)) return kDeduceValueType_Char;
    return kDeduceValueType_Real_Number;
}

DeduceValue operator+(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() != kDeduceValueType_Real_Number &&
        n.GetType() != kDeduceValueType_Real_Number) {
        return DeduceValue(i.AsInt() + n.AsInt());
    } else {
        return DeduceValue(i.AsReal() + n.AsReal());
    }
}

DeduceValue operator-(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() != kDeduceValueType_Real_Number &&
        n.GetType() != kDeduceValueType_Real_Number) {
        return DeduceValue(i.AsInt() - n.AsInt());
    } else {
        return DeduceValue(i.AsReal() - n.AsReal());
    }
}

DeduceValue operator*(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() != kDeduceValueType_Real_Number &&
        n.GetType() != kDeduceValueType_Real_Number) {
        return DeduceValue(i.AsInt() * n.AsInt());
    } else {
        return DeduceValue(i.AsReal() * n.AsReal());
    }
}

DeduceValue operator/(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() != kDeduceValueType_Real_Number &&
        n.GetType() != kDeduceValueType_Real_Number) {
        return DeduceValue(i.AsInt() / n.AsInt());
    } else {
        return DeduceValue(i.AsReal() / n.AsReal());
    }
}

DeduceValue operator%(DeduceValue i, const DeduceValue& n) {
    return DeduceValue(i.AsInt() % n.AsInt());
}

}  // namespace hzcc::ast
