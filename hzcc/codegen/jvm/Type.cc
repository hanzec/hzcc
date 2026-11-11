//
// Created by hanzech on 5/23/22.
//

#include "Type.h"

#include "AST/type/ArrayType.h"
namespace Hzcc::Codegen::JVM {

/**
 * @brief Map an already normalized primitive spelling to a JVM instruction prefix.
 * @param typename_ Unqualified primitive spelling recognized by this helper.
 * @param replace_char Map char to the integer prefix when true.
 * @return Lowercase instruction type prefix for the recognized primitive.
 * @pre typename_ names one of the explicitly supported primitive types.
 */
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

/**
 * @brief Remove recognized keyword substrings and pointer, reference, and array punctuation.
 * @param typename_ Type spelling copied before normalization.
 * @return A copy with each matched keyword occurrence and all *, &, brackets and spaces removed.
 * @note Keyword removal uses substring matching and is not limited to leading qualifiers.
 */
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

/**
 * @brief Initialize Type with the supplied backend state.
 * @param type_name Borrowed AST type handle; it must outlive this object.
 */
Type::Type(const std::shared_ptr<AST::Type> &type_name)
    : _type_name(type_name),
      _raw_type_name(RemoveAttribute(type_name->GetName())) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(type_name != nullptr)
        << "type_name is nullptr, please check your code";
}

/**
 * @brief Test whether the retained AST type is void.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsVoid() const { return _type_name->IsVoid(); }
/**
 * @brief Test whether the retained AST type is char.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsChar() const { return _type_name->IsChar(); }
/**
 * @brief Test whether the retained AST type is float.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsFloat() const { return _type_name->IsFloat(); }
/**
 * @brief Test whether the retained AST type is double.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsDouble() const { return _type_name->IsDouble(); }
/**
 * @brief Test whether the retained AST type is int.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsInt() const { return _type_name->IsInteger(); }
/**
 * @brief Test whether the retained AST type is long.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsLong() const { return _type_name->IsLong(); }
/**
 * @brief Test whether the retained AST type is short.
 * @return True when the stated condition holds; otherwise false.
 */
bool Type::IsShort() const { return _type_name->IsShort(); }
/**
 * @brief Return the retained AST type name.
 * @return The original AST type spelling.
 */
std::string Type::GetName() const { return _type_name->GetName(); }

/**
 * @brief Return the JVM descriptor for the retained AST type.
 * @param replace_char Use the integer representation for character values when true.
 * @return JVM type character, prefixed with [ for an array.
 */
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
