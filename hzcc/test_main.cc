//
// Created by Hanze Chen on 2022/1/23.
//

#include <gtest/gtest.h>

/**
 * @brief Initialize GoogleTest and execute the registered unit tests.
 * @param argc Argument count, updated when GoogleTest removes its options.
 * @param argv Argument vector consumed by GoogleTest.
 * @return GoogleTest process status: zero when all selected tests pass.
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
