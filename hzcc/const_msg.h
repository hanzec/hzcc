#include <array>

#include "version.h"

#ifndef HZCC_LEXICAL_KEYWORDS_H
#define HZCC_LEXICAL_KEYWORDS_H
namespace hzcc {
constexpr const char *kMsg_Author =
    "hzcc - A C compiler\n"
    "\tWritten by Hanze Chen (tony.chen424@gmail.com)\n"
    "\tVersion " FULL_VERSION
    "\n"
    "\tBuild Time " BUILD_TIMESTAMP;
}  // namespace hzcc
#endif  // HZCC_LEXICAL_KEYWORDS_H
