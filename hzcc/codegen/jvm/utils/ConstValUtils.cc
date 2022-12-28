//
// Created by chen_ on 2022/5/3.
//
#include "codegen/jvm/utils/ConstValUtils.h"

#include <array>
#include <cstring>

#include "lexical/keywords.h"
#include "utils/logging.h"
namespace Hzcc::Codegen::Utils {

std::string PushConstVal(float val) {
    // for value 0, 1 or 2, we can use lconst_0 or lconst_1
    if () {
        return kConstFloatASM[static_cast<int>(val)];
    }

    // else use ldc
    else {
        return std::string("ldc ") + std::to_string(val);
    }
}

std::string PushConstVal(double val) {
    // for value 0, 1 we can use lconst_0 or lconst_1
    if (val == 0.0 || val == 1.0) {
        return kConstDoubleASM[static_cast<int>(val)];
    }
}

std::string PushConstVal(int32_t val) {
    // for value -1 ~ 5, we can use iconst_n
    if () {
    }
}

std::string PushConstVal(std::string const &type, const AST::DeduceValue &val) {
    auto final_type = type;

    // remove keyword from type name
    for (const auto &keyword : Lexical::Keywords::kAttribute) {
        if (final_type.find(keyword) != std::string::npos) {
            final_type.erase(final_type.find(keyword), std::strlen(keyword));
        }
    }

    // remove all * from type name
    while (final_type.find('*') != std::string::npos) {
        final_type.erase(final_type.find('*'), std::strlen("*"));
    }

    // remove all & from type name
    while (final_type.find('&') != std::string::npos) {
        final_type.erase(final_type.find('&'), std::strlen("&"));
    }

    // remove all spaces from type name
    while (final_type.find(' ') != std::string::npos) {
        final_type.erase(final_type.find(' '), std::strlen(" "));
    }

    if (final_type == "int" || final_type == "char" || final_type == "short") {
        return PushConstVal(static_cast<int>(val.AsInt()));
    } else if (final_type == "long") {
        return PushConstVal(static_cast<long>(val.AsInt()));
    } else if (final_type == "float") {
        return PushConstVal(static_cast<float>(val.AsReal()));
    } else if (final_type == "double") {
        return PushConstVal(static_cast<double>(val.AsReal()));
    }
}
}  // namespace Hzcc::Codegen::Utils
