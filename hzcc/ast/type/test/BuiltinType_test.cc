//
// Created by chen_ on 2022/6/13.
//
#include <gtest/gtest.h>

namespace hzcc::ast {
TEST(AST_TYPE, builtin_type_void) {
    BuiltinType void_type(PrimitiveType::kvoid);

    EXPECT_EQ(void_type.Name(), "void");
    EXPECT_TRUE(void_type.is());
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
    BuiltinType char_type(PrimitiveType::kchar);

    EXPECT_EQ(char_type.Name(), "char");
    EXPECT_TRUE(char_type.IsChar());
    EXPECT_FALSE(char_type.is());
    EXPECT_FALSE(char_type.IsInteger());
    EXPECT_FALSE(char_type.IsFloat());
    EXPECT_FALSE(char_type.IsDouble());
    EXPECT_FALSE(char_type.IsLong());
    EXPECT_FALSE(char_type.IsShort());
    EXPECT_FALSE(char_type.IsLongLong());
    EXPECT_FALSE(char_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_short) {
    BuiltinType int_type(PrimitiveType::kshort);

    EXPECT_EQ(int_type.Name(), "short");
    EXPECT_FALSE(int_type.IsChar());
    EXPECT_FALSE(int_type.is());
    EXPECT_FALSE(int_type.IsInteger());
    EXPECT_FALSE(int_type.IsFloat());
    EXPECT_FALSE(int_type.IsDouble());
    EXPECT_FALSE(int_type.IsLong());
    EXPECT_TRUE(int_type.IsShort());
    EXPECT_FALSE(int_type.IsLongLong());
    EXPECT_FALSE(int_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_int) {
    BuiltinType int_type(PrimitiveType::kint);

    EXPECT_EQ(int_type.Name(), "int");
    EXPECT_FALSE(int_type.IsChar());
    EXPECT_FALSE(int_type.is());
    EXPECT_TRUE(int_type.IsInteger());
    EXPECT_FALSE(int_type.IsFloat());
    EXPECT_FALSE(int_type.IsDouble());
    EXPECT_FALSE(int_type.IsLong());
    EXPECT_FALSE(int_type.IsShort());
    EXPECT_FALSE(int_type.IsLongLong());
    EXPECT_FALSE(int_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_long) {
    BuiltinType long_type(PrimitiveType::klong);

    EXPECT_EQ(long_type.Name(), "long");
    EXPECT_FALSE(long_type.IsChar());
    EXPECT_FALSE(long_type.is());
    EXPECT_FALSE(long_type.IsInteger());
    EXPECT_FALSE(long_type.IsFloat());
    EXPECT_FALSE(long_type.IsDouble());
    EXPECT_TRUE(long_type.IsLong());
    EXPECT_FALSE(long_type.IsShort());
    EXPECT_FALSE(long_type.IsLongLong());
    EXPECT_FALSE(long_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_long_long) {
    BuiltinType longlong_type(PrimitiveType::klonglong);

    EXPECT_EQ(longlong_type.Name(), "long long");
    EXPECT_FALSE(longlong_type.IsChar());
    EXPECT_FALSE(longlong_type.is());
    EXPECT_FALSE(longlong_type.IsInteger());
    EXPECT_FALSE(longlong_type.IsFloat());
    EXPECT_FALSE(longlong_type.IsDouble());
    EXPECT_FALSE(longlong_type.IsLong());
    EXPECT_FALSE(longlong_type.IsShort());
    EXPECT_TRUE(longlong_type.IsLongLong());
    EXPECT_FALSE(longlong_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_float) {
    BuiltinType float_type(PrimitiveType::kfloat);

    EXPECT_EQ(float_type.Name(), "float");
    EXPECT_FALSE(float_type.IsChar());
    EXPECT_FALSE(float_type.is());
    EXPECT_FALSE(float_type.IsInteger());
    EXPECT_TRUE(float_type.IsFloat());
    EXPECT_FALSE(float_type.IsDouble());
    EXPECT_FALSE(float_type.IsLong());
    EXPECT_FALSE(float_type.IsShort());
    EXPECT_FALSE(float_type.IsLongLong());
    EXPECT_FALSE(float_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_double) {
    hzcc::ast::BuiltinType double_type(PrimitiveType::kdouble);

    EXPECT_EQ(double_type.Name(), "double");
    EXPECT_FALSE(double_type.IsChar());
    EXPECT_FALSE(double_type.is());
    EXPECT_FALSE(double_type.IsInteger());
    EXPECT_FALSE(double_type.IsFloat());
    EXPECT_TRUE(double_type.IsDouble());
    EXPECT_FALSE(double_type.IsLong());
    EXPECT_FALSE(double_type.IsShort());
    EXPECT_FALSE(double_type.IsLongLong());
    EXPECT_FALSE(double_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_long_double) {
    hzcc::ast::BuiltinType bool_type(PrimitiveType::klong_double);

    EXPECT_EQ(bool_type.Name(), "long double");
    EXPECT_FALSE(bool_type.IsChar());
    EXPECT_FALSE(bool_type.is());
    EXPECT_FALSE(bool_type.IsInteger());
    EXPECT_FALSE(bool_type.IsFloat());
    EXPECT_FALSE(bool_type.IsDouble());
    EXPECT_FALSE(bool_type.IsLong());
    EXPECT_FALSE(bool_type.IsShort());
    EXPECT_FALSE(bool_type.IsLongLong());
    EXPECT_TRUE(bool_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_bool) {
    hzcc::ast::BuiltinType bool_type(PrimitiveType::kbool);

    EXPECT_EQ(bool_type.Name(), "bool");
    EXPECT_FALSE(bool_type.IsChar());
    EXPECT_FALSE(bool_type.is());
    EXPECT_FALSE(bool_type.IsInteger());
    EXPECT_FALSE(bool_type.IsFloat());
    EXPECT_FALSE(bool_type.IsDouble());
    EXPECT_FALSE(bool_type.IsLong());
    EXPECT_FALSE(bool_type.IsShort());
    EXPECT_FALSE(bool_type.IsLongLong());
    EXPECT_FALSE(bool_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_complex) {
    hzcc::ast::BuiltinType complex_type(PrimitiveType::kcomplex);

    EXPECT_EQ(complex_type.Name(), "complex");
    EXPECT_FALSE(complex_type.IsChar());
    EXPECT_FALSE(complex_type.is());
    EXPECT_FALSE(complex_type.IsInteger());
    EXPECT_FALSE(complex_type.IsFloat());
    EXPECT_FALSE(complex_type.IsDouble());
    EXPECT_FALSE(complex_type.IsLong());
    EXPECT_FALSE(complex_type.IsShort());
    EXPECT_FALSE(complex_type.IsLongLong());
    EXPECT_FALSE(complex_type.IsLongDouble());
}

TEST(AST_TYPE, builtin_type_imaginary) {
    hzcc::ast::BuiltinType imaginary_type(PrimitiveType::kimaginary);

    EXPECT_EQ(imaginary_type.Name(), "imaginary");
    EXPECT_FALSE(imaginary_type.IsChar());
    EXPECT_FALSE(imaginary_type.is());
    EXPECT_FALSE(imaginary_type.IsInteger());
    EXPECT_FALSE(imaginary_type.IsFloat());
    EXPECT_FALSE(imaginary_type.IsDouble());
    EXPECT_FALSE(imaginary_type.IsLong());
    EXPECT_FALSE(imaginary_type.IsShort());
    EXPECT_FALSE(imaginary_type.IsLongLong());
    EXPECT_FALSE(imaginary_type.IsLongDouble());
}  // namespace hzcc::ast