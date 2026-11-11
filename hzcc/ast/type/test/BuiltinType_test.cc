#include <gtest/gtest.h>

#include "ast/type/Type.h"

namespace hzcc::ast {
namespace {
/**
 * @brief Verify primitive spelling, category, and distinction from other primitive IDs.
 * @param primitive Primitive ID represented by the numerical type.
 * @param spelling Expected C spelling, including underscores in C99 type keywords.
 */
void ExpectNumericalType(PrimitiveType primitive, const char* spelling) {
    const NumericalType type(primitive);
    EXPECT_EQ(type.to_str(), spelling);
    EXPECT_TRUE(type.is<TypeCategory::Numerical>());
    EXPECT_EQ(type.GetTypeId(), magic_enum::enum_integer(primitive));

    for (const auto other : magic_enum::enum_values<PrimitiveType>()) {
        const NumericalType other_type(other);
        EXPECT_EQ(type == other_type, primitive == other);
        EXPECT_EQ(type != other_type, primitive != other);
    }
}
}  // namespace

/** @brief Check the spelling and primitive identity of void. */
TEST(AST_TYPE, builtin_type_void) {
    ExpectNumericalType(PrimitiveType::kVoid, "void");
}

/** @brief Check the spelling and primitive identity of char. */
TEST(AST_TYPE, builtin_type_char) {
    ExpectNumericalType(PrimitiveType::kChar, "char");
}

/** @brief Check the spelling and primitive identity of short. */
TEST(AST_TYPE, builtin_type_short) {
    ExpectNumericalType(PrimitiveType::kShort, "short");
}

/** @brief Check the spelling and primitive identity of int. */
TEST(AST_TYPE, builtin_type_int) {
    ExpectNumericalType(PrimitiveType::kInt, "int");
}

/** @brief Check the spelling and primitive identity of long. */
TEST(AST_TYPE, builtin_type_long) {
    ExpectNumericalType(PrimitiveType::kLong, "long");
}

/** @brief Check the spelling and primitive identity of long long. */
TEST(AST_TYPE, builtin_type_long_long) {
    // Preserve the existing numerical-type table mapping of this legacy enum slot.
    ExpectNumericalType(PrimitiveType::Signed, "long long");
}

/** @brief Check the spelling and primitive identity of float. */
TEST(AST_TYPE, builtin_type_float) {
    ExpectNumericalType(PrimitiveType::kFloat, "float");
}

/** @brief Check the spelling and primitive identity of double. */
TEST(AST_TYPE, builtin_type_double) {
    ExpectNumericalType(PrimitiveType::kDouble, "double");
}

/** @brief Check the spelling and primitive identity of long double. */
TEST(AST_TYPE, builtin_type_long_double) {
    // Preserve the existing numerical-type table mapping of this legacy enum slot.
    ExpectNumericalType(PrimitiveType::Unsigned, "long double");
}

/** @brief Check the spelling and primitive identity of _Bool. */
TEST(AST_TYPE, builtin_type_bool) {
    ExpectNumericalType(PrimitiveType::kBool, "_Bool");
}

/** @brief Check the spelling and primitive identity of _Complex. */
TEST(AST_TYPE, builtin_type_complex) {
    ExpectNumericalType(PrimitiveType::kComplex, "_Complex");
}

/** @brief Check the spelling and primitive identity of _Imaginary. */
TEST(AST_TYPE, builtin_type_imaginary) {
    ExpectNumericalType(PrimitiveType::kImaginary, "_Imaginary");
}

}  // namespace hzcc::ast
