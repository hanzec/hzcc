#ifndef HZCC_UTILS_INTEGER_LITERAL_H
#define HZCC_UTILS_INTEGER_LITERAL_H

#include <charconv>
#include <cstdint>
#include <limits>
#include <string_view>

namespace hzcc::utils {
/** @brief Radix and conversion outcome for an integer literal spelling. */
struct IntegerLiteralResult {
    int base;
    std::from_chars_result conversion;
};

/** @brief Read a signed 64-bit integer in the compiler's decimal, octal, hex or binary
 * form.
 * @param spelling Source spelling, optionally preceded by a sign.
 * @param value Receives the result only when the entire spelling is valid and in range.
 * @return Detected radix and a pointer to the first unconverted character, with the
 * conversion error if the digits are missing, malformed or out of range.
 */
inline IntegerLiteralResult parse_integer_literal(std::string_view spelling,
                                                  int64_t& value) noexcept {
    if (spelling.empty()) return {10, {spelling.data(), std::errc::invalid_argument}};
    const bool negative = spelling.front() == '-';
    if (negative || spelling.front() == '+') spelling.remove_prefix(1);
    int base = 10;
    if (spelling.size() > 1 && spelling.front() == '0') {
        switch (spelling[1]) {
            case 'x':
            case 'X':
                base = 16;
                spelling.remove_prefix(2);
                break;
            case 'b':
            case 'B':
                base = 2;
                spelling.remove_prefix(2);
                break;
            default:
                base = 8;
        }
    }

    uint64_t magnitude = 0;
    const char* end = spelling.data() + spelling.size();
    auto result = std::from_chars(spelling.data(), end, magnitude, base);
    if (result.ptr != end) result.ec = std::errc::invalid_argument;
    if (result.ec != std::errc{}) return {base, result};

    constexpr auto kMax = static_cast<uint64_t>((std::numeric_limits<int64_t>::max)());
    if (magnitude > kMax + negative) return {base, {end, std::errc::result_out_of_range}};
    if (magnitude > kMax) {
        value = (std::numeric_limits<int64_t>::min)();
    } else {
        value =
            negative ? -static_cast<int64_t>(magnitude) : static_cast<int64_t>(magnitude);
    }
    return {base, result};
}
}  // namespace hzcc::utils
#endif  // HZCC_UTILS_INTEGER_LITERAL_H
