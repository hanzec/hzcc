//
// Created by chen_ on 2022/4/10.
//

#include "TypeUtils.h"

#include <cstring>
#include <string>

#include "AST/type/ArrayType.h"
#include "AST/type/Type.h"
#include "lexical/keywords.h"
namespace Hzcc::Codegen::Utils {

std::string RemoveAttribute(const std::string &typename_) {
    auto final_type = typename_;

    // remove keyword from type name
    for (const auto &keyword : Lexical::Keywords::kKeyword) {
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

    // remove all [] from type name
    while (final_type.find('[') != std::string::npos) {
        final_type.erase(final_type.find('['), std::strlen("["));
    }
    while (final_type.find(']') != std::string::npos) {
        final_type.erase(final_type.find(']'), std::strlen("]"));
    }

    return final_type;
}

inline char GetJVMTypename(const std::string &typename_,
                           bool replace_char = false) {
    auto final_type = RemoveAttribute(typename_);

    // get actual type name
    if (final_type == "int") {
        return 'i';
    } else if (final_type == "long") {
        return 'l';
    } else if (final_type == "float") {
        return 'f';
    } else if (final_type == "double") {
        return 'd';
    } else if (final_type == "boolean") {
        return 'z';
    } else if (final_type == "char") {
        if (replace_char) {
            return 'i';
        } else {
            return 'c';
        }
    } else if (final_type == "byte") {
        return 'b';
    } else if (final_type == "short") {
        return 's';
    } else if (final_type == "void") {
        return 'v';
    } else {
        DLOG_ASSERT(false) << "unknown type: " << typename_;
    }
}

std::string GetTypeName(const std::shared_ptr<AST::Type> &type,
                        bool replace_char) {
    // if type is a array type, return the array type name
    if (type->IsArray()) {
        auto array_type = std::dynamic_pointer_cast<AST::ArrayType>(type);
        return "[" + std::string(
                         1, GetJVMTypename(array_type->GetBaseType()->GetName(),
                                           replace_char));
    } else {
        return std::string(1, GetJVMTypename(type->GetName(), replace_char));
    }
}
}  // namespace Hzcc::Codegen::Utils
