//
// Created by Hanze Chen on 2022/1/26.
//

#ifndef HZCC_MACRO_H
#define HZCC_MACRO_H

/** #########################################################################
 *  #### Useful Typedefs   ##################################################
 *  ######################################################################### */
namespace hzcc {
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

#if defined(__clang__) && __has_feature(warn_unused_result)
#define HZCC_MUST_USE_RESULT __attribute__((warn_unused_result))
#else
#define HZCC_MUST_USE_RESULT
#endif

#if __has_feature(nonnull) || (defined(__GNUC__) && !defined(__clang__))
#define ABSL_ATTRIBUTE_NONNULL(arg_index) __attribute__((nonnull(arg_index)))
#else
#define HZCC_ATTRIBUTE_NONNULL(...)
#endif

// ABSL_ATTRIBUTE_LIFETIME_BOUND indicates that a resource owned by a function
// parameter or implicit object parameter is retained by the return value of the
// annotated function (or, for a parameter of a constructor, in the value of the
// constructed object). This attribute causes warnings to be produced if a
// temporary object does not live long enough.
//
// When applied to a reference parameter, the referenced object is assumed to be
// retained by the return value of the function. When applied to a non-reference
// parameter (for example, a pointer or a class type), all temporaries
// referenced by the parameter are assumed to be retained by the return value of
// the function.
//
// See also the upstream documentation:
// https://clang.llvm.org/docs/AttributeReference.html#lifetimebound
#if __has_cpp_attribute(clang::lifetimebound)
#define HZCC_ATTRIBUTE_LIFETIME_BOUND [[clang::lifetimebound]]
#elif __has_attribute(lifetimebound)
#define HZCC_ATTRIBUTE_LIFETIME_BOUND __attribute__((lifetimebound))
#else
#define HZCC_ATTRIBUTE_LIFETIME_BOUND
#endif

#endif  // HZCC_MACRO_H
