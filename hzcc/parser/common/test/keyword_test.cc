//
// Created by chen_ on 2023/2/10.
//
#include <gtest/gtest.h>

#include "parser/common/name_utils.h"
namespace hzcc::parser_common {
TEST(PARSER_COMMON_TOKENTYPE, literal) {
    // 302 ~ 306 is reserved for literal
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Char_Lit), 302);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Int_Lit), 303);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Real_Lit), 304);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Str_Lit), 305);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kIdentity), 306);
}

TEST(PARSER_COMMON_TOKENTYPE, type_specifier) {
    // 310 ~ 322 is reserved for type specifier
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Int), 310);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Char), 311);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Float), 312);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Double), 313);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Void), 314);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Long), 315);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Short), 316);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Bool), 317);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Complex), 318);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Imaginary), 319);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Signed), 320);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Unsigned), 321);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Auto), 322);
}

TEST(PARSER_COMMON_TOKENTYPE, Operator) {
    // 351 ~ 372 is reserved for operator
    EXPECT_EQ(magic_enum::enum_integer(TokenType::EQ), 351);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::NE), 352);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::GE), 353);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::LE), 354);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kSelfIncrement), 355);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kSelfDecrement), 356);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kLogicalOr), 357);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kLogicalAnd), 358);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kLeftShift), 359);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kRightShift), 360);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kAddAssign), 361);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kSubAssign), 362);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kMulAssign), 363);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kDivAssign), 364);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kArrow), 365);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kSelfMinus), 366);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kModAssign), 367);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kLShiftAssign), 368);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kRShiftAssign), 369);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kAndAssign), 370);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kXorAssign), 371);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::kOrAssign), 372);
}

TEST(PARSER_COMMON_TOKENTYPE, qualifier) {
    // 400 ~ 403 is reserved for qualifier
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Const), 400);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Volatile), 401);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Restrict), 402);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Atomic), 403);
}

TEST(PARSER_COMMON_TOKENTYPE, keyword) {
    // 500 ~ 519 is reserved for keyword
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Break), 500);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Case), 501);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Continue), 502);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Default), 503);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Do), 504);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Else), 505);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Enum), 506);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Extern), 507);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::For), 508);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Goto), 509);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::If), 510);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Return), 511);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Sizeof), 512);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Struct), 513);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Switch), 514);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Typedef), 515);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Union), 516);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::While), 517);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Alignas), 518);
    EXPECT_EQ(magic_enum::enum_integer(TokenType::Alignof), 519);
}
}  // namespace hzcc::parser_common