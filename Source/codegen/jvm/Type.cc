//
// Created by hanzech on 5/23/22.
//

#include "Type.h"

#include "AST/type/ArrayType.h"
namespace Hzcc::Codegen::JVM {

static inline char GetJVMTypename(const std::string &typename_,
                                  bool replace_char = false) {
    // get actual type name
    if (typename_ == "int") {
        return 'i';
    } else if (typename_ == "long") {
        return 'l';
    } else if (typename_ == "float") {
        return 'f';
    } else if (typename_ == "double") {
        return 'd';
    } else if (typename_ == "boolean") {
        return 'z';
    } else if (typename_ == "char") {
        if (replace_char) {
            return 'i';
        } else {
            return 'c';
        }
    } else if (typename_ == "byte") {
        return 'b';
    } else if (typename_ == "short") {
        return 's';
    } else if (typename_ == "void") {
        return 'v';
    } else {
        DLOG_ASSERT(false) << "unknown type: " << typename_;
    }
}

static inline std::string RemoveAttribute(const std::string &typename_) {
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

Type::Type(const std::shared_ptr<AST::Type> &type_name)
    : _type_name(type_name),
      _raw_type_name(RemoveAttribute(type_name->GetName())) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(type_name != nullptr)
        << "type_name is nullptr, please check your code";
}

bool Type::IsVoid() const { return _type_name->IsVoid(); }
bool Type::IsChar() const { return _type_name->IsChar(); }
bool Type::IsFloat() const { return _type_name->IsFloat(); }
bool Type::IsDouble() const { return _type_name->IsDouble(); }
bool Type::IsInt() const { return _type_name->IsInteger(); }
bool Type::IsLong() const { return _type_name->IsLong(); }
bool Type::IsShort() const { return _type_name->IsShort(); }
std::string Type::GetName() const { return _type_name->GetName(); }

std::string Type::GetTypeIdentifier(
    bool replace_char) {  // if type is a array type, return the array
                          // type name
    if (_type_name->IsArray()) {
        auto array_type = std::dynamic_pointer_cast<AST::ArrayType>(_type_name);
        return "[" +
               std::string(1, GetJVMTypename(_raw_type_name, replace_char));
    } else {
        // No check here, use initializer will cause error
        return std::string(                                 // NOLINT
            1,                                              // NOLINT
            GetJVMTypename(_raw_type_name, replace_char));  // NOLINT
    }
}

}  // namespace Hzcc::Codegen::JVM
