//
// Created by chen_ on 2022/1/28.
//

#ifndef HZCC_FS_UTILS_H
#define HZCC_FS_UTILS_H

#include <filesystem>

#include "macro.h"
namespace hzcc::FsUtils {
/** @brief Inspect whether all owner, group, and other read-permission bits are set.
 * @param path Filesystem entry whose permission bits are queried.
 * @return Whether all three read bits are present. This is not an effective-user access check.
 */
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
}  // namespace hzcc::FsUtils
#endif  // HZCC_FS_UTILS_H
