//
// Created by chen_ on 2022/1/28.
//

#ifndef HZCC_FS_UTILS_H
#define HZCC_FS_UTILS_H

#include <filesystem>
#include <fstream>

#include "macro.h"
namespace hzcc::FsUtils {
/** @brief Check whether the current process can open a regular file for reading.
 * @param path Filesystem path to test.
 * @return True when the path is a regular file and a read-only stream opens it.
 */
bool ALWAYS_INLINE is_readable(const std::filesystem::path& path) {
    std::error_code ec;  // For noexcept overload usage.
    return std::filesystem::is_regular_file(path, ec) && std::ifstream(path).is_open();
}
}  // namespace hzcc::FsUtils
#endif  // HZCC_FS_UTILS_H
