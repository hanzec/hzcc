//
// Created by chen_ on 2022/1/28.
//

#ifndef MYCC_FS_UTILS_H
#define MYCC_FS_UTILS_H

#include <filesystem>

#include "macro.h"
namespace Mycc::FsUtils {

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
}  // namespace Mycc::FsUtils
#endif  // MYCC_FS_UTILS_H
