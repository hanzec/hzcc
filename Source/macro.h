//
// Created by chen_ on 2022/1/26.
//

#ifndef MYCC_MACRO_H
#define MYCC_MACRO_H

#ifndef __has_attribute         // Optional of course.
#define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#if __has_attribute(packed)
#define PACKED __attribute__((packed))
#else
#define PACKED
#endif

#endif  // MYCC_MACRO_H
