//
// Created by chen_ on 2022/3/26.
//
#include <string_view>

#include "fnv1a_hash.h"
#ifndef HZCC_SOURCE_UTILS_TYPE_NAME_UTILS_H_
#define HZCC_SOURCE_UTILS_TYPE_NAME_UTILS_H_
namespace Hzcc {
struct TypeNameUtil {
    size_t name_hash{0};
    size_t matcher_hash{0};

    bool operator==(const TypeNameUtil& other) const {
        return name_hash == other.name_hash;
    }
    template <typename T>
    static constexpr const char* name_detail() {
#if RSTUDIO_CORE_PLATFORM == RSTUDIO_CORE_PLATFORM_LINUX
        return __PRETTY_FUNCTION__;
#else
        return __FUNCSIG__;
#endif
    }

#if RSTUDIO_CORE_PLATFORM != RSTUDIO_CORE_PLATFORM_WIN32
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

    template <typename T>
    static constexpr size_t hash() noexcept {
        static_assert(!std::is_reference_v<T>, "dont send references to hash");
        static_assert(!std::is_const_v<T>, "dont send const to hash");
        return hash_64_fnv1a_const(name_detail<T>());
    }
};
}  // namespace Hzcc
#endif  // HZCC_SOURCE_UTILS_TYPE_NAME_UTILS_H_
