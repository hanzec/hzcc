//
// Created by chen_ on 2022/6/13.
//
#include "AST/type/BuiltinType.h"

#include <gtest/gtest.h>

namespace Hzcc::AST {
TEST(AST_TYPE, builtin_type_void) {
    BuiltinType void_type(kPrimitiveType_void);

    EXPECT_EQ(void_type.Name(), "void");
    EXPECT_TRUE(void_type.IsVoid());
    EXPECT_FALSE(void_type.IsLong());
    EXPECT_FALSE(void_type.IsShort());
    EXPECT_FALSE(void_type.IsChar());
    EXPECT_FALSE(void_type.IsFloat());
    EXPECT_FALSE(void_type.IsDouble());
    EXPECT_FALSE(void_type.IsInteger());
    EXPECT_FALSE(void_type.IsLongLong());
    EXPECT_FALSE(void_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_char) {
    BuiltinType char_type(kPrimitiveType_char);

    EXPECT_EQ(char_type.Name(), "char");
    EXPECT_TRUE(char_type.IsChar());
    EXPECT_FALSE(char_type.IsVoid());
    EXPECT_FALSE(char_type.IsInteger());
    EXPECT_FALSE(char_type.IsFloat());
    EXPECT_FALSE(char_type.IsDouble());
    EXPECT_FALSE(char_type.IsLong());
    EXPECT_FALSE(char_type.IsShort());
    EXPECT_FALSE(char_type.IsLongLong());
    EXPECT_FALSE(char_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_short) {
    BuiltinType int_type(kPrimitiveType_short);

    EXPECT_EQ(int_type.Name(), "short");
    EXPECT_FALSE(int_type.IsChar());
    EXPECT_FALSE(int_type.IsVoid());
    EXPECT_FALSE(int_type.IsInteger());
    EXPECT_FALSE(int_type.IsFloat());
    EXPECT_FALSE(int_type.IsDouble());
    EXPECT_FALSE(int_type.IsLong());
    EXPECT_TRUE(int_type.IsShort());
    EXPECT_FALSE(int_type.IsLongLong());
    EXPECT_FALSE(int_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_int) {
    BuiltinType int_type(kPrimitiveType_int);

    EXPECT_EQ(int_type.Name(), "int");
    EXPECT_FALSE(int_type.IsChar());
    EXPECT_FALSE(int_type.IsVoid());
    EXPECT_TRUE(int_type.IsInteger());
    EXPECT_FALSE(int_type.IsFloat());
    EXPECT_FALSE(int_type.IsDouble());
    EXPECT_FALSE(int_type.IsLong());
    EXPECT_FALSE(int_type.IsShort());
    EXPECT_FALSE(int_type.IsLongLong());
    EXPECT_FALSE(int_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_long) {
    BuiltinType long_type(kPrimitiveType_long);

    EXPECT_EQ(long_type.Name(), "long");
    EXPECT_FALSE(long_type.IsChar());
    EXPECT_FALSE(long_type.IsVoid());
    EXPECT_FALSE(long_type.IsInteger());
    EXPECT_FALSE(long_type.IsFloat());
    EXPECT_FALSE(long_type.IsDouble());
    EXPECT_TRUE(long_type.IsLong());
    EXPECT_FALSE(long_type.IsShort());
    EXPECT_FALSE(long_type.IsLongLong());
    EXPECT_FALSE(long_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_long_long) {
    BuiltinType longlong_type(kPrimitiveType_longlong);

    EXPECT_EQ(longlong_type.Name(), "long long");
    EXPECT_FALSE(longlong_type.IsChar());
    EXPECT_FALSE(longlong_type.IsVoid());
    EXPECT_FALSE(longlong_type.IsInteger());
    EXPECT_FALSE(longlong_type.IsFloat());
    EXPECT_FALSE(longlong_type.IsDouble());
    EXPECT_FALSE(longlong_type.IsLong());
    EXPECT_FALSE(longlong_type.IsShort());
    EXPECT_TRUE(longlong_type.IsLongLong());
    EXPECT_FALSE(longlong_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_float) {
    BuiltinType float_type(kPrimitiveType_float);

    EXPECT_EQ(float_type.Name(), "float");
    EXPECT_FALSE(float_type.IsChar());
    EXPECT_FALSE(float_type.IsVoid());
    EXPECT_FALSE(float_type.IsInteger());
    EXPECT_TRUE(float_type.IsFloat());
    EXPECT_FALSE(float_type.IsDouble());
    EXPECT_FALSE(float_type.IsLong());
    EXPECT_FALSE(float_type.IsShort());
    EXPECT_FALSE(float_type.IsLongLong());
    EXPECT_FALSE(float_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_double) {
    BuiltinType double_type(kPrimitiveType_double);

    EXPECT_EQ(double_type.Name(), "double");
    EXPECT_FALSE(double_type.IsChar());
    EXPECT_FALSE(double_type.IsVoid());
    EXPECT_FALSE(double_type.IsInteger());
    EXPECT_FALSE(double_type.IsFloat());
    EXPECT_TRUE(double_type.IsDouble());
    EXPECT_FALSE(double_type.IsLong());
    EXPECT_FALSE(double_type.IsShort());
    EXPECT_FALSE(double_type.IsLongLong());
    EXPECT_FALSE(double_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_long_double) {
    BuiltinType bool_type(kPrimitiveType_long_double);

    EXPECT_EQ(bool_type.Name(), "long double");
    EXPECT_FALSE(bool_type.IsChar());
    EXPECT_FALSE(bool_type.IsVoid());
    EXPECT_FALSE(bool_type.IsInteger());
    EXPECT_FALSE(bool_type.IsFloat());
    EXPECT_FALSE(bool_type.IsDouble());
    EXPECT_FALSE(bool_type.IsLong());
    EXPECT_FALSE(bool_type.IsShort());
    EXPECT_FALSE(bool_type.IsLongLong());
    EXPECT_TRUE(bool_type.IsLongDouble());
}

}  // namespace Hzcc::AST