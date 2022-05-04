//
// Created by chen_ on 2022/4/10.
//

#include "TypeUtils.h"

#include <cstring>

#include "lexical/keywords.h"
namespace Hzcc::Codegen::Utils  {

char GetJVMTypename(const std::string &typename_) {
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

    // get actual type name
    if (typename_ == "int") {
        return 'I';
    } else if (typename_ == "float") {
        return 'F';
    } else if (typename_ == "double") {
        return 'D';
    } else if (typename_ == "long") {
        return 'J';
    } else if (typename_ == "char") {
        return 'C';
    } else if (typename_ == "boolean") {
        return 'Z';
    } else if (typename_ == "void") {
        return 'V';
    } else if (typename_ == "byte") {
        return 'B';
    } else if (typename_ == "short") {
        return 'S';
    } else {
        return 'L';
    }
}

}  // namespace Hzcc::Codegen
