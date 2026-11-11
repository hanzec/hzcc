//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_NAME_UTILS_H
#define HZCC_NAME_UTILS_H
#include "enums.h"
#include "keywords.h"

namespace hzcc::parser_common {
namespace detail {
/**
 * @brief Locate a type-specifier spelling by content rather than pointer identity.
 * @param str Exact character range to compare; it need not be null-terminated.
 * @return First matching index in kTypeSpecifier, or -1 when absent.
 */
constexpr ALWAYS_INLINE int PrimitiveTypeIndex(std::string_view str) {
    for (std::size_t index = 0; index < kTypeSpecifier.size(); ++index) {
        if (std::string_view(kTypeSpecifier[index]) == str) {
            return static_cast<int>(index);
        }
    }
    return -1;
}
}  // namespace detail

/**
 * @brief Check a spelling against the primitive type-specifier table.
 * @param str Exact character range of the spelling to classify.
 * @return True when the spelling appears in kTypeSpecifier.
 */
ALWAYS_INLINE bool is_primitive_type(std::string_view str) {
    return detail::PrimitiveTypeIndex(str) != -1;
}

/**
 * @brief Check a character-array spelling against the type-specifier table.
 * @tparam N Array extent, including the terminating null character.
 * @param str Null-terminated spelling to classify.
 * @return True when the spelling appears in kTypeSpecifier.
 */
template <std::size_t N>
constexpr ALWAYS_INLINE bool IsPrimitiveType(const char (&str)[N]) {
    return detail::PrimitiveTypeIndex(std::string_view(str, N - 1)) != -1;
}

/**
 * @brief Resolve a type-specifier spelling to a primitive type identifier.
 * @param str Exact spelling of a supported primitive type.
 * @return Matching PrimitiveType; an unsupported spelling triggers a fatal log.
 */
constexpr ALWAYS_INLINE PrimitiveType GetPrimitiveType(std::string_view str) {
    auto ret = magic_enum::enum_cast<PrimitiveType>(detail::PrimitiveTypeIndex(str));
    if (!ret.has_value()) {
        LOG(FATAL) << "Invalid PrimitiveType";
    }
    return ret.value();
}

/**
 * @brief Resolve a character-array spelling to a primitive type identifier.
 * @tparam N Character-array extent, including its terminator.
 * @param str Null-terminated spelling with a matching PrimitiveType entry.
 * @return Primitive identifier selected by the spelling; valid lookups are constexpr.
 */
template <std::size_t N>
constexpr ALWAYS_INLINE PrimitiveType GetPrimitiveType(const char (&str)[N]) {
    return GetPrimitiveType(std::string_view(str, N - 1));
}
}  // namespace hzcc::parser_common
#endif  // HZCC_NAME_UTILS_H
