//
// Created by chen_ on 2022/12/24.
//
#include <gtest/gtest.h>

#include "parser/common/name_utils.h"

namespace hzcc::parser_common {

TEST(PARSER_COMMON_NAME_UTILS, token_type_to_attr_type){
    EXPECT_EQ(TokenTypeToAttr(kTokenType_ATTRIBUTE_START), kSTART);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_ATTRIBUTE_END), kEND);

    EXPECT_EQ(TokenTypeToAttr(kTokenType_Const), kCONST);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Extern), kEXTERN);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Static), kSTATIC);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Auto), kAUTO);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Volatile), kVOLATILE);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Restrict), kRESTRICT);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Inline), kINLINE);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Signed), kSIGNED);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Unsigned), kUNSIGNED);
    EXPECT_EQ(TokenTypeToAttr(kTokenType_Register), kREGISTER);
}

}