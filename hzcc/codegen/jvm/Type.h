//
// Created by hanzech on 5/23/22.
//
#include "AST/type/Type.h"

#ifndef HZCC_CODEGEN_JVM_JVM_TYPE_H
#define HZCC_CODEGEN_JVM_JVM_TYPE_H
namespace Hzcc::Codegen::JVM {
/**
 * @brief Represent a legacy AST type in JVM descriptors and instruction type prefixes.
 */
class Type {
  public:
    /**
     * @brief Initialize Type with the supplied backend state.
     * @param type_name Borrowed AST type handle; it must outlive this object.
     */
    explicit Type(const std::shared_ptr<AST::Type>& type_name);

    /**
     * @brief Return the JVM descriptor for the retained AST type.
     * @param replace_char Use the integer representation for character values when true.
     * @return JVM type character, prefixed with [ for an array.
     */
    std::string GetTypeIdentifier(bool replace_char);

    /**
     * @brief Test whether the retained AST type is void.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsVoid() const;

    /**
     * @brief Test whether the retained AST type is char.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsChar() const;

    /**
     * @brief Test whether the retained AST type is float.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsFloat() const;

    /**
     * @brief Test whether the retained AST type is double.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsDouble() const;

    /**
     * @brief Test whether the retained AST type is int.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsInt() const;

    /**
     * @brief Test whether the retained AST type is long.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsLong() const;

    /**
     * @brief Test whether the retained AST type is short.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsShort() const;

    /**
     * @brief Return the retained AST type name.
     * @return The original AST type spelling.
     */
    [[nodiscard]] std::string GetName() const;

  private:
    const std::string _raw_type_name;
    const std::shared_ptr<AST::Type>& _type_name;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // HZCC_CODEGEN_JVM_JVM_TYPE_H
