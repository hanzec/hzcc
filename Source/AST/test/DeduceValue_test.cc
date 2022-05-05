//
// Created by chen_ on 2022/5/4.
//
//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#include "AST/DeduceValue.h"

namespace Hzcc::AST {
TEST(AST_NODE_DeduceValue, char_literal_0) {  // NOLINT
   char val = 'a';
   DeduceValue deduce_value(val);

   EXPECT_EQ(deduce_value.AsChar(), val);
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
}  // namespace Hzcc::Lexical
