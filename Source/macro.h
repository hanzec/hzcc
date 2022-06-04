//
// Created by Hanze Chen on 2022/1/26.
//

#ifndef HZCC_MACRO_H
#define HZCC_MACRO_H

/** #########################################################################
 *  #### Useful Typedefs   ##################################################
 *  ######################################################################### */
namespace Hzcc {
using Position = std::pair<uint_fast32_t, uint_fast32_t>;
}

/** #########################################################################
 *  #### COMPILER FEATURES ##################################################
 *  ######################################################################### */
#ifndef __has_feature       // Optional of course.
#define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_attribute       // Optional of course.
#define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
#define __has_extension __has_feature  // Compatibility with pre-3.0 compilers.
#endif

#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define ALWAYS_INLINE
#endif

#if __has_attribute(packed)
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#if defined(__cpp_lib_filesystem) && __has_include(<filesystem>)
#define HAS_STD_FILESYSTEM
#endif
#endif  // HZCC_MACRO_H
