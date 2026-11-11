#include "utils/cached_list.h"

#include <gtest/gtest.h>

#include <list>
#include <stdexcept>

namespace hzcc::utils {
namespace {
/// @brief Test predicate treating negative integer tokens as qualifiers.
struct IsQualifier {
    /** @brief Classify a test token.
     * @param value Integer token to inspect.
     * @return Whether the token is negative and should be cached.
     */
    bool operator()(int value) const { return value < 0; }
};
using Tokens = CachedList<int, IsQualifier>;

TEST(CachedListTest, ConstructsEmptyListOrCopiesExistingList) {
    Tokens empty(IsQualifier{});
    EXPECT_TRUE(empty.empty());

    std::list<int> source{-1, 10, 20};
    Tokens tokens(IsQualifier{}, source);
    EXPECT_EQ(tokens.pop(), 10);
    EXPECT_EQ(tokens.size(), 1U);
    EXPECT_EQ(source, (std::list<int>{-1, 10, 20}));
    EXPECT_EQ(tokens.cache_attr_list(), (std::list<int>{-1}));
}

TEST(CachedListTest, LookaheadSkipsQualifiersWithoutConsumingOrCaching) {
    std::list<int> source{-1, -2, 10, -3, 20, -4, 30};
    Tokens tokens(IsQualifier{}, source);
    const Tokens& view = tokens;

    EXPECT_EQ(view.peek(), 10);
    EXPECT_EQ(view.peek2(), 20);
    EXPECT_EQ(view.peek3(), 30);
    EXPECT_EQ(view.peek(), 10);
    EXPECT_EQ(std::list<int>(tokens.begin(), tokens.end()), source);
    EXPECT_TRUE(tokens.cache_attr_list().empty());
}

TEST(CachedListTest, PopAccumulatesQualifiersInOrderAndCacheReadDrainsThem) {
    std::list<int> source{-1, -2, 10, -3, 20, 30};
    Tokens tokens(IsQualifier{}, source);

    EXPECT_EQ(tokens.pop(), 10);
    EXPECT_EQ(tokens.pop(), 20);
    EXPECT_EQ(tokens.cache_attr_list(), (std::list<int>{-1, -2, -3}));
    EXPECT_TRUE(tokens.cache_attr_list().empty());
    EXPECT_EQ(tokens.pop(), 30);
    EXPECT_TRUE(tokens.cache_attr_list().empty());
    EXPECT_TRUE(tokens.empty());
}

TEST(CachedListTest, EmptyListRejectsConsumptionAndLookahead) {
    Tokens tokens(IsQualifier{});
    EXPECT_THROW(tokens.pop(), std::out_of_range);
    EXPECT_THROW((void)tokens.peek(), std::out_of_range);
    EXPECT_THROW((void)tokens.peek2(), std::out_of_range);
    EXPECT_THROW((void)tokens.peek3(), std::out_of_range);
    EXPECT_TRUE(tokens.empty());
    EXPECT_TRUE(tokens.cache_attr_list().empty());
}

TEST(CachedListTest, LookaheadChecksVisibleTokenCount) {
    std::list<int> source{-1, 10, -2, 20, -3};
    Tokens tokens(IsQualifier{}, source);

    EXPECT_EQ(tokens.peek2(), 20);
    EXPECT_THROW((void)tokens.peek3(), std::out_of_range);
    EXPECT_EQ(std::list<int>(tokens.begin(), tokens.end()), source);
    EXPECT_EQ(tokens.pop(), 10);
    EXPECT_EQ(tokens.peek(), 20);
    EXPECT_THROW((void)tokens.peek2(), std::out_of_range);
    EXPECT_EQ(tokens.pop(), 20);
    EXPECT_THROW((void)tokens.peek(), std::out_of_range);
}

TEST(CachedListTest, AllFilteredInputIsPreservedOnFailure) {
    std::list<int> source{-1, -2};
    Tokens tokens(IsQualifier{}, source);

    EXPECT_THROW((void)tokens.peek(), std::out_of_range);
    EXPECT_THROW((void)tokens.peek2(), std::out_of_range);
    EXPECT_THROW((void)tokens.peek3(), std::out_of_range);
    EXPECT_THROW(tokens.pop(), std::out_of_range);
    EXPECT_EQ(std::list<int>(tokens.begin(), tokens.end()), source);
    EXPECT_FALSE(tokens.empty());
    EXPECT_TRUE(tokens.cache_attr_list().empty());
}

TEST(CachedListTest, FailedPopPreservesPreviouslyCachedQualifiers) {
    std::list<int> source{-1, 10, -2};
    Tokens tokens(IsQualifier{}, source);

    EXPECT_EQ(tokens.pop(), 10);
    EXPECT_THROW(tokens.pop(), std::out_of_range);
    EXPECT_EQ(tokens.cache_attr_list(), (std::list<int>{-1}));
    EXPECT_EQ(std::list<int>(tokens.begin(), tokens.end()), (std::list<int>{-2}));
}

TEST(CachedListTest, PushRestoresFrontTokenWithoutChangingCache) {
    std::list<int> source{-1, 10, 20, 30};
    Tokens tokens(IsQualifier{}, source);

    tokens.push(tokens.pop());
    EXPECT_EQ(tokens.peek(), 10);
    EXPECT_EQ(tokens.peek2(), 20);
    EXPECT_EQ(tokens.peek3(), 30);
    EXPECT_EQ(tokens.cache_attr_list(), (std::list<int>{-1}));
}
}  // namespace
}  // namespace hzcc::utils
