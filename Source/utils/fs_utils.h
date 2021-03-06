//
// Created by chen_ on 2022/1/28.
//

#ifndef HZCC_FS_UTILS_H
#define HZCC_FS_UTILS_H

#include <filesystem>

#include "macro.h"
namespace Hzcc::FsUtils {
bool ALWAYS_INLINE is_readable(const std::filesystem::path& path) {
    std::error_code ec;  // For noexcept overload usage.
    auto perms = std::filesystem::status(path, ec).permissions();
    return ((perms & std::filesystem::perms::owner_read) !=
                std::filesystem::perms::none &&
            (perms & std::filesystem::perms::group_read) !=
                std::filesystem::perms::none &&
            (perms & std::filesystem::perms::others_read) !=
                std::filesystem::perms::none);
}
}  // namespace Hzcc::FsUtils
#endif  // HZCC_FS_UTILS_H
