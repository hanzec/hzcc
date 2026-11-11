#include <gtest/gtest.h>

#include <string>

#include "parser/common/name_utils.h"

namespace hzcc::parser_common {

/** @test Qualifier tokens map to the corresponding qualified-type attributes. */
TEST(PARSER_COMMON_NAME_UTILS, token_type_to_attr_type) {
    EXPECT_EQ(to_attr(TokenType::Const), Qualifier::CONST);
    EXPECT_EQ(to_attr(TokenType::Volatile), Qualifier::VOLATILE);
    EXPECT_EQ(to_attr(TokenType::Restrict), Qualifier::RESTRICT);
    EXPECT_EQ(to_attr(TokenType::Atomic), Qualifier::ATOMIC);
}

/** @test Primitive type lookup recognizes supported names and rejects identifiers. */
TEST(PARSER_COMMON_NAME_UTILS, primitive_type_names) {
    EXPECT_TRUE(is_primitive_type("int"));
    EXPECT_TRUE(is_primitive_type("_Bool"));
    EXPECT_FALSE(is_primitive_type("integer"));
    EXPECT_FALSE(is_primitive_type("const"));
    EXPECT_EQ(GetPrimitiveType(std::string_view("int")), PrimitiveType::kInt);
    EXPECT_EQ(GetPrimitiveType(std::string_view("char")), PrimitiveType::kChar);
    EXPECT_EQ(GetPrimitiveType(std::string_view("double")), PrimitiveType::kDouble);
    const std::string dynamic_name = "double";
    EXPECT_TRUE(is_primitive_type(dynamic_name));
    EXPECT_EQ(GetPrimitiveType(std::string_view(dynamic_name)), PrimitiveType::kDouble);
    const std::string padded_name = "int suffix";
    EXPECT_TRUE(is_primitive_type(std::string_view(padded_name).substr(0, 3)));
    EXPECT_FALSE(is_primitive_type(padded_name));
    static_assert(IsPrimitiveType("float"));
    static_assert(GetPrimitiveType("float") == PrimitiveType::kFloat);
    static_assert(!IsPrimitiveType("floaty"));
}

}  // namespace hzcc::parser_common
