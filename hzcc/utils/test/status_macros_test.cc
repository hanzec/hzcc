#include <gtest/gtest.h>

#include "utils/logging.h"
#include "utils/status/statusor.h"

namespace hzcc {
namespace {

/**
 * @brief Exercise the assignment macro using both populated and failed StatusOr values.
 * @param input Result to forward through the macro.
 * @return The same integer or error status supplied by the caller.
 */
StatusOr<int> ForwardStatus(StatusOr<int> input) {
    HZCC_CHECK_OR_ASSIGN(value, std::move(input));
    return value;
}

/** @test Successful StatusOr assignment forwards its stored value. */
TEST(STATUS_MACROS, forwards_value) {
    auto result = ForwardStatus(42);
    ASSERT_TRUE(result.ok());
    EXPECT_EQ(result.value(), 42);
}

/** @test Error propagation does not dereference a StatusOr without a value. */
TEST(STATUS_MACROS, forwards_error_without_value_access) {
    auto result = ForwardStatus(UnimplementedError("parser not registered"));
    EXPECT_TRUE(IsUnimplemented(result.status()));
    EXPECT_EQ(result.status().message(), "parser not registered");
}

}  // namespace
}  // namespace hzcc
