//
// Created by chen_ on 2022/3/29.
//

#include "value.h"

#include <cstring>
namespace Mycc::AST {

DeduceValue::DeduceValue(uint64_t value) : _type(kInt) {
    int64_t val = value;
    memcpy(&_raw_value, &val, sizeof(int64_t));
}

DeduceValue::DeduceValue(double value) : _type(kReal_Number) {
    memccpy(&_raw_value, &value, sizeof(double), sizeof(double));
}

DeduceValue::DeduceValue(DeduceValue::Type type, std::string str_value)
    : _type(type) {
    if (type == DeduceValue::Type::kInt) {
        // if hex, convert to int
        if (str_value.size() > 2 && str_value[0] == '0' &&
            str_value[1] == 'x') {
            int64_t int_val = std::stoi(str_value, nullptr, 16);
            std::memcpy(&_raw_value, &int_val, sizeof(int_val));
        }

        // if oct, convert to int
        else if (str_value.size() > 1 && str_value[0] == '0') {
            int64_t int_val = std::stoi(str_value, nullptr, 8);
            std::memcpy(&_raw_value, &int_val, sizeof(int_val));
        }

        // if decimal, convert to int
        else {
            int64_t int_val = std::stoi(str_value);
            std::memcpy(&_raw_value, &int_val, sizeof(int_val));
        }
    } else {
        // convert as double
        double double_val = std::stod(str_value);
        std::memcpy(&_raw_value, &double_val, sizeof(double_val));
    }
}
int64_t DeduceValue::AsInt() const {
    if (_type == DeduceValue::Type::kInt) {
        return _raw_value;
    } else {
        double double_val = 0;
        std::memcpy(&double_val, &_raw_value, sizeof(double_val));
        return static_cast<int64_t>(double_val);
    }
}

double DeduceValue::AsReal() const {
    if (_type == DeduceValue::Type::kReal_Number) {
        double double_val = 0;
        std::memcpy(&double_val, &_raw_value, sizeof(double_val));
        return double_val;
    } else {
        int64_t int_val = 0;
        std::memcpy(&int_val, &_raw_value, sizeof(int_val));
        return static_cast<double>(int_val);
    }
}
DeduceValue::Type DeduceValue::GetType() const { return _type; }

DeduceValue operator+(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() == DeduceValue::Type::kInt &&
        n.GetType() == DeduceValue::Type::kInt) {
        return DeduceValue((uint64_t)(i.AsInt() + n.AsInt()));
    } else {
        return DeduceValue(i.AsReal() + n.AsReal());
    }
}

DeduceValue operator-(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() == DeduceValue::Type::kInt &&
        n.GetType() == DeduceValue::Type::kInt) {
        return DeduceValue((uint64_t)(i.AsInt() - n.AsInt()));
    } else {
        return DeduceValue(i.AsReal() - n.AsReal());
    };
}

DeduceValue operator*(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() == DeduceValue::Type::kInt &&
        n.GetType() == DeduceValue::Type::kInt) {
        return DeduceValue((uint64_t)(i.AsInt() * n.AsInt()));
    } else {
        return DeduceValue(i.AsReal() * n.AsReal());
    };
}

DeduceValue operator/(DeduceValue i, const DeduceValue& n) {
    if (i.GetType() == DeduceValue::Type::kInt &&
        n.GetType() == DeduceValue::Type::kInt) {
        return DeduceValue((uint64_t)(i.AsInt() / n.AsInt()));
    } else {
        return DeduceValue(i.AsReal() / n.AsReal());
    };
}

DeduceValue operator%(DeduceValue i, const DeduceValue& n) {
    return DeduceValue((uint64_t)(i.AsInt() / n.AsInt()));
}

}  // namespace Mycc::AST