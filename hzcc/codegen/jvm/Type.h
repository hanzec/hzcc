//
// Created by hanzech on 5/23/22.
//
#include "AST/type/Type.h"

#ifndef HZCC_CODEGEN_JVM_JVM_TYPE_H
#define HZCC_CODEGEN_JVM_JVM_TYPE_H
namespace Hzcc::Codegen::JVM {
class Type {
  public:
    explicit Type(const std::shared_ptr<AST::Type>& type_name);

    std::string GetTypeIdentifier(bool replace_char);

    /**
     * @brief Check if the type is void
     * @return True if the type is void, false otherwise.
     */
    [[nodiscard]] bool IsVoid() const;

    /**
     * @brief Check if the type is character
     * @return True if the type is character, false otherwise.
     */
    [[nodiscard]] bool IsChar() const;

    /**
     * @brief Check if the type is Float
     * @return True if the type is Float, false otherwise.
     */
    [[nodiscard]] bool IsFloat() const;

    /**
     * @brief Check if the type is Double
     * @return True if the type is Double, false otherwise.
     */
    [[nodiscard]] bool IsDouble() const;

    /**
     * @brief Check if the type is Integer
     * @return True if the type is Integer, false otherwise.
     */
    [[nodiscard]] bool IsInt() const;

    /**
     * @brief Check if the type is Long
     * @return True if the type is Long, false otherwise.
     */
    [[nodiscard]] bool IsLong() const;

    /**
     * @brief Check if the type is Short
     * @return True if the type is Short, false otherwise.
     */
    [[nodiscard]] bool IsShort() const;

    [[nodiscard]] std::string GetName() const;

  private:
    const std::string _raw_type_name;
    const std::shared_ptr<AST::Type>& _type_name;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // HZCC_CODEGEN_JVM_JVM_TYPE_H
