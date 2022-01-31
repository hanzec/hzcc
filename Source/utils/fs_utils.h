//
// Created by chen_ on 2022/1/28.
//

#ifndef MYCC_FS_UTILS_H
#define MYCC_FS_UTILS_H

#ifdef HAS_STD_FILESYSTEM
#  include <filesystem>
#else
#  include <iostream>
#endif

#include "macro.h"
namespace Mycc {

bool ALWAYS_INLINE is_readable(const std::filesystem::path& path) {
#ifdef HAS_STD_FILESYSTEM
  std::error_code ec;  // For noexcept overload usage.
  auto perms = std::filesystem::status(path, ec).permissions();
  return ((perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none &&
          (perms & std::filesystem::perms::group_read) != std::filesystem::perms::none &&
          (perms & std::filesystem::perms::others_read) != std::filesystem::perms::none);
#else
  std::ifstream ifs(path.string());
  return ifs.good();
#endif
}
}  // namespace Mycc
#endif  // MYCC_FS_UTILS_H
