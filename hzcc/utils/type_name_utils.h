//
// Created by chen_ on 2022/3/26.
//
#include <string_view>

#include "fnv1a_hash.h"
#ifndef HZCC_SOURCE_UTILS_TYPE_NAME_UTILS_H_
#define HZCC_SOURCE_UTILS_TYPE_NAME_UTILS_H_
namespace hzcc {
/** @brief Compiler-signature hashes used to identify C++ types.
 * Hashes depend on the compiler and are not a stable serialization format.
 */
struct TypeNameUtil {
    size_t name_hash{0};
    size_t matcher_hash{0};

    /** @brief Compare type-name hashes; matcher_hash is not part of equality.
     * @param other Type identity to compare.
     * @return Whether both name_hash values match.
     */
    bool operator==(const TypeNameUtil& other) const {
        return name_hash == other.name_hash;
    }
    /** @brief Obtain the compiler-generated signature containing T.
     * @tparam T Type whose compiler signature is requested.
     * @return Pointer to compiler-owned static signature text.
     */
    template <typename T>
    static constexpr const char* name_detail() {
#ifdef _MSC_FULL_VER  // Microsoft
        return __FUNCSIG__;
#else
        return __PRETTY_FUNCTION__;
#endif
    }

#ifdef _MSC_FULL_VER  // Microsoft
    /** @brief Strip the legacy signature prefix and suffix from name_detail().
     * @tparam T Type named in the signature.
     * @return A view into the static signature string.
     * @note The fixed offsets assume the signature layout in this branch.
     */
    template <typename T>
    static constexpr auto name_pretty() {
        // name_detail() is like: static constexpr const char*
        // ecs::TypeNameUtil::name_detail() [with T = rstudio::math::Vector3]
        std::string_view pretty_name = name_detail<T>();
        std::string_view prefix =
            "static constexpr const char* ecs::TypeNameUtil::name_detail() "
            "[with T = ";
        std::string_view suffix = "]";
        pretty_name.remove_prefix(prefix.size());
        pretty_name.remove_suffix(suffix.size());
        return pretty_name;
    }
#else
    /** @brief Strip the legacy signature prefix from name_detail().
     * @tparam T Type named in the signature.
     * @return A view into the static signature string.
     * @note The fixed offset assumes the signature layout in this branch.
     */
    template <typename T>
    static constexpr auto name_pretty() {
        // name_detail() is like "const char *__cdecl
        // ecs::TypeNameUtil::name_detail<class rstudio::math::Vector3>(void)"
        std::string_view pretty_name = name_detail<T>();
        std::string_view prefix =
            "const char *__cdecl ecs::TypeNameUtil::name_detail() ";

        pretty_name.remove_prefix(prefix.size());
        return pretty_name;
    }
#endif

    /** @brief Hash the complete compiler signature of an unqualified type.
     * @tparam T Non-reference, non-const type to identify.
     * @return FNV-1a hash of name_detail<T>().
     */
    template <typename T>
    static constexpr size_t hash() noexcept {
        static_assert(!std::is_reference_v<T>, "dont send references to hash");
        static_assert(!std::is_const_v<T>, "dont send const to hash");
        return hash_64_fnv1a_const(name_detail<T>());
    }
};
}  // namespace hzcc
#endif  // HZCC_SOURCE_UTILS_TYPE_NAME_UTILS_H_
