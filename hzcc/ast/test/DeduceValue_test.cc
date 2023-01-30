//
// Created by chen_ on 2022/5/4.
//
//
// Created by chen_ on 2022/2/3.
//
#include "AST/DeduceValue.h"

#include <gtest/gtest.h>

namespace hzcc::ast {
TEST(AST_NODE_DeduceValue, char_literal_0) {  // NOLINT
    char val = 'a';
    DeduceValue deduce_value(val);

    EXPECT_EQ(deduce_value.as_char(), val);
    EXPECT_EQ(deduce_value.AsInt(), val);
    EXPECT_EQ(deduce_value.AsReal(), val);
    EXPECT_EQ(deduce_value.GetType(), DeduceValueType::kDeduceValueType_Char);
}
TEST(AST_NODE_DeduceValue, int_literal_0) {  // NOLINT
    int64_t val = 33;
    DeduceValue deduce_value(val);

    EXPECT_EQ(deduce_value.AsInt(), val);
    EXPECT_EQ(deduce_value.AsReal(), val);
    EXPECT_EQ(deduce_value.GetType(), DeduceValueType::kDeduceValueType_Int);
}
}  // namespace hzcc::ast
