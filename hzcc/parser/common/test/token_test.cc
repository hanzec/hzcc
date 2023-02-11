//
// Created by chen_ on 2022/2/3.
//
#include <gtest/gtest.h>

#define private public  // hack compiler allow asserting private member
#define protected public
#include "parser/common/Token.h"
#undef private
#undef protected

namespace hzcc::parser_common {
TEST(PARSER_COMMON_TOKEN, construct_one_with_string_test) {  // NOLINT
    EXPECT_EQ(Token::_const_value_storage.size(),
              0);  // const vale pool should be empty

    auto* new_token = new Token("test", kIdentity, 0, 0);

    // const vale pool should have one element
    EXPECT_EQ(Token::_const_value_storage.size(), 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->second, 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->first, "test");

    // token should be constructed
    EXPECT_EQ(new_token->to_str(), "test");
    EXPECT_EQ(new_token->type(), kIdentity);
    EXPECT_EQ(new_token->loc(), std::make_pair(0, 0));

    delete new_token;
    // const vale pool should be empty when object is deleted
    EXPECT_EQ(Token::_const_value_storage.size(), 0);
}

TEST(PARSER_COMMON_TOKEN, construct_one_without_string_test) {  // NOLINT
    EXPECT_EQ(Token::_const_value_storage.size(),
              0);  // const vale pool should be empty

    auto* new_token = new Token(kAdd, 0, 0);

    // const vale pool should have no element
    EXPECT_EQ(Token::_const_value_storage.size(), 0);

    // token should be constructed
    EXPECT_EQ(new_token->to_str(), "+");
    EXPECT_EQ(new_token->type(), kAdd);
    EXPECT_EQ(new_token->loc(), std::make_pair(0, 0));

    delete new_token;
    // const vale pool should be empty when object is deleted
    EXPECT_EQ(Token::_const_value_storage.size(), 0);
}

TEST(PARSER_COMMON_TOKEN, construct_two_with_same_string_test) {  // NOLINT
    EXPECT_EQ(Token::_const_value_storage.size(),
              0);  // const vale pool should be empty

    auto* new_token_0 = new Token("test", kIdentity, 0, 0);

    // const vale pool should have one element after first construction
    EXPECT_EQ(Token::_const_value_storage.size(), 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->second, 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->first, "test");

    // token should be constructed
    EXPECT_EQ(new_token_0->to_str(), "test");
    EXPECT_EQ(new_token_0->type(), kIdentity);
    EXPECT_EQ(new_token_0->loc(), std::make_pair(0, 0));

    auto* new_token_1 = new Token("test", kIdentity, 0, 1);

    // const vale pool should have one element after second construction
    EXPECT_EQ(Token::_const_value_storage.size(), 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->second, 2);
    EXPECT_EQ(Token::_const_value_storage.begin()->first, "test");

    // token should be constructed
    EXPECT_EQ(new_token_1->to_str(), "test");
    EXPECT_EQ(new_token_1->type(), kIdentity);
    EXPECT_EQ(new_token_1->loc(), std::make_pair(0, 1));

    delete new_token_0;

    // ref count should decrease by 1
    EXPECT_EQ(Token::_const_value_storage.size(), 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->second, 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->first, "test");

    // new token should be still available now
    EXPECT_EQ(new_token_1->to_str(), "test");

    delete new_token_1;

    // const vale pool should be empty when all object is deleted
    EXPECT_EQ(Token::_const_value_storage.size(), 0);
}

TEST(PARSER_COMMON_TOKEN, construct_three_string_test) {  // NOLINT
    EXPECT_EQ(Token::_const_value_storage.size(),
              0);  // const vale pool should be empty

    auto* new_token_0 = new Token("test", kIdentity, 0, 0);

    // const vale pool should have one element after first construction
    EXPECT_EQ(Token::_const_value_storage.size(), 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->second, 1);
    EXPECT_EQ(Token::_const_value_storage.begin()->first, "test");

    // token should be constructed
    EXPECT_EQ(new_token_0->to_str(), "test");
    EXPECT_EQ(new_token_0->type(), kIdentity);
    EXPECT_EQ(new_token_0->loc(), std::make_pair(0, 0));

    auto* new_token_1 = new Token("test_other", kIdentity, 0, 1);

    // const vale pool should have one element after second construction
    EXPECT_EQ(Token::_const_value_storage.size(), 2);
    EXPECT_EQ(Token::_const_value_storage["test"], 1);
    EXPECT_EQ(Token::_const_value_storage["test_other"], 1);

    // token should be constructed
    EXPECT_EQ(new_token_1->to_str(), "test_other");
    EXPECT_EQ(new_token_1->type(), kIdentity);
    EXPECT_EQ(new_token_1->loc(), std::make_pair(0, 1));

    auto* new_token_2 = new Token("test", kIdentity, 0, 1);

    // const vale pool should have one element after second construction
    EXPECT_EQ(Token::_const_value_storage.size(), 2);
    EXPECT_EQ(Token::_const_value_storage["test"], 2);
    EXPECT_EQ(Token::_const_value_storage["test_other"], 1);

    // token should be constructed
    EXPECT_EQ(new_token_2->to_str(), "test");
    EXPECT_EQ(new_token_2->type(), kIdentity);
    EXPECT_EQ(new_token_2->loc(), std::make_pair(0, 1));

    delete new_token_0;

    // ref count should decrease by 1
    EXPECT_EQ(Token::_const_value_storage.size(), 2);
    EXPECT_EQ(Token::_const_value_storage["test"], 1);
    EXPECT_EQ(Token::_const_value_storage["test_other"], 1);

    // new token should be still available now
    EXPECT_EQ(new_token_1->to_str(), "test_other");
    EXPECT_EQ(new_token_2->to_str(), "test");

    delete new_token_2;

    // ref count should decrease by 1
    EXPECT_EQ(Token::_const_value_storage.size(), 1);
    EXPECT_EQ(Token::_const_value_storage["test_other"], 1);

    // new token should be still available now
    EXPECT_EQ(new_token_1->to_str(), "test_other");

    delete new_token_1;

    // const vale pool should be empty when all object is deleted
    EXPECT_EQ(Token::_const_value_storage.size(), 0);
}

}  // namespace hzcc::lexical
