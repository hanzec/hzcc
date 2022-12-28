//
// Created by chen_ on 2022/3/29.
//

#include "DeduceValue.h"

#include <cstring>

#include "utils/logging.h"
namespace hzcc::ast {

DeduceValue::DeduceValue(char value) : _type(kDeduceValueType_Char) {
    std::memcpy(&_raw_value.front(), &value, sizeof(char));
}

DeduceValue::DeduceValue(int64_t value) : _type(kDeduceValueType_Int) {
    std::memcpy(&_raw_value.front(), &value, sizeof(int64_t));
}

DeduceValue::DeduceValue(double value) : _type(kDeduceValueType_Real_Number) {
    std::memcpy(_raw_value.data(), &value, sizeof(double));
}

DeduceValue::DeduceValue(DeduceValueType type, std::string str_value)
    : _type(type) {
    switch (type) {
        case kDeduceValueType_Int:
            // if hex, convert to int
            if (str_value.size() > 2 && str_value[0] == '0' &&
                str_value[1] == 'x') {
                int64_t int_val = std::stoi(str_value, nullptr, 16);
                std::memcpy(_raw_value.data(), &int_val, sizeof(int_val));
            }

            // if oct, convert to int
            else if (str_value.size() > 1 && str_value[0] == '0') {
                int64_t int_val = std::stoi(str_value, nullptr, 8);
                std::memcpy(_raw_value.data(), &int_val, sizeof(int_val));
            }

            // if decimal, convert to int
            else {
                int64_t int_val = std::stoi(str_value);
                std::memcpy(_raw_value.data(), &int_val, sizeof(int_val));
            }
            break;
        case kDeduceValueType_Char: {
            std::memcpy(&_raw_value[3], &str_value[0], sizeof(char));
        } break;
        case kDeduceValueType_Real_Number: {  // convert as double
            double double_val = std::stod(str_value);
            std::memcpy(_raw_value.data(), &double_val, sizeof(double_val));
        } break;
        default:
            DLOG_ASSERT(false) << "unknown type";
    }
}

char DeduceValue::AsChar() const {
    DLOG_ASSERT(_type == kDeduceValueType_Char) << "type is not char";
    char ret;
    std::memcpy(&ret, &_raw_value.front(), sizeof(char));
    return ret;
}

int64_t DeduceValue::AsInt() const {
    if (_type == DeduceValueType::kDeduceValueType_Int ||
        _type == DeduceValueType::kDeduceValueType_Char) {
        int64_t ret;
        std::memcpy(&ret, _raw_value.data(), sizeof(int64_t));
        return ret;
    } else {
        double double_val = 0;
        std::memcpy(&double_val, &_raw_value, sizeof(double_val));
        return static_cast<int64_t>(double_val);
    }
}

double DeduceValue::AsReal() const {
    if (_type == DeduceValueType::kDeduceValueType_Real_Number) {
        double double_val = 0;
        std::memcpy(&double_val, &_raw_value, sizeof(double_val));
        return double_val;
    } else {
        int64_t int_val = 0;
        std::memcpy(&int_val, &_raw_value, sizeof(int_val));
        return static_cast<double>(int_val);
    }
}

DeduceValueType DeduceValue::GetType() const { return _type; }

DeduceValue operator+(DeduceValue i, const DeduceValue& n) {
    if ((i.GetType() == DeduceValueType::kDeduceValueType_Int &&
         n.GetType() == DeduceValueType::kDeduceValueType_Int) ||
        (i.GetType() == DeduceValueType::kDeduceValueType_Char &&
         n.GetType() == DeduceValueType::kDeduceValueType_Char)) {
        return DeduceValue(i.AsInt() + n.AsInt());
    } else {
        return DeduceValue(i.AsReal() + n.AsReal());
    }
}

DeduceValue operator-(DeduceValue i, const DeduceValue& n) {
    if ((i.GetType() == DeduceValueType::kDeduceValueType_Int &&
         n.GetType() == DeduceValueType::kDeduceValueType_Int) ||
        (i.GetType() == DeduceValueType::kDeduceValueType_Char &&
         n.GetType() == DeduceValueType::kDeduceValueType_Char)) {
        return DeduceValue(i.AsInt() - n.AsInt());
    } else {
        return DeduceValue(i.AsReal() - n.AsReal());
    }
}

DeduceValue operator*(DeduceValue i, const DeduceValue& n) {
    if ((i.GetType() == DeduceValueType::kDeduceValueType_Int &&
         n.GetType() == DeduceValueType::kDeduceValueType_Int) ||
        (i.GetType() == DeduceValueType::kDeduceValueType_Char &&
         n.GetType() == DeduceValueType::kDeduceValueType_Char)) {
        return DeduceValue(i.AsInt() * n.AsInt());
    } else {
        return DeduceValue(i.AsReal() * n.AsReal());
    }
}

DeduceValue operator/(DeduceValue i, const DeduceValue& n) {
    if ((i.GetType() == DeduceValueType::kDeduceValueType_Int &&
         n.GetType() == DeduceValueType::kDeduceValueType_Int) ||
        (i.GetType() == DeduceValueType::kDeduceValueType_Char &&
         n.GetType() == DeduceValueType::kDeduceValueType_Char)) {
        return DeduceValue(i.AsInt() / n.AsInt());
    } else {
        return DeduceValue(i.AsReal() / n.AsReal());
    }
}

DeduceValue operator%(DeduceValue i, const DeduceValue& n) {
    return DeduceValue(i.AsInt() / n.AsInt());
}

}  // namespace hzcc::ast
