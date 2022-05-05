//
// Created by chen_ on 2022/5/3.
//
#include "codegen/jvm/utils/ConstValUtils.h"

#include <array>
#include <cstring>

#include "lexical/keywords.h"
#include "utils/logging.h"
namespace Hzcc::Codegen::Utils {
constexpr static std::array<const char *, 7> kConstIntASM = {
    "iconst_M1", "iconst_0", "iconst_1", "iconst_2",
    "iconst_3",  "iconst_4", "iconst_5"};
constexpr static std::array<const char *, 3> kConstFloatASM = {
    "fconst_0", "fconst_1", "fconst_2"};
constexpr static std::array<const char *, 3> kConstDoubleASM = {"dconst_0",
                                                                "dconst_1"};
constexpr static std::array<const char *, 2> kConstLongASM = {"lconst_0",
                                                              "lconst_1"};

std::string PushConstVal(long val) {
    // for value 1 or 0 , we can use lconst_0 or lconst_1
    if (val == 0 || val == 1) {
        return kConstLongASM[val];
    }

    // if its between 127 and -128, we can use bipush
    else if (val >= -128 && val <= 127) {
        return std::string("bipush ") + std::to_string(val);
    }

    // if its between 32767 and -32768, we can use lconst_n
    else if (val >= -32768 && val <= 32767) {
        return std::string("sipush ") + std::to_string(val);
    }

    // else use ldc
    else {
        return std::string("ldc ") + std::to_string(val);
    }
}

std::string PushConstVal(float val) {
    // for value 0, 1 or 2, we can use lconst_0 or lconst_1
    if (val == 0.0f || val == 1.0f || val == 2.0f) {
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

    // else use ldc
    else {
        return std::string("ldc ") + std::to_string(val);
    }
}

std::string PushConstVal(int32_t val) {
    // for value -1 ~ 5, we can use iconst_n
    if (val >= -1 && val <= 5) {
        return kConstIntASM[val + 1];
    }

    // if its between 127 and -128, we can use bipush
    else if (val >= -128 && val <= 127) {
        return std::string("bipush ") + std::to_string(val);
    }

    // if its between 32767 and -32768, we can use sipush
    else if (val >= -32768 && val <= 32767) {
        return std::string("sipush ") + std::to_string(val);
    }

    // else use ldc
    else {
        return std::string("ldc ") + std::to_string(val);
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
    } else {
        LOG(FATAL) << "Unsupported type: " << final_type
                   << " with original type: " << type;
    }
}
}  // namespace Hzcc::Codegen::Utils