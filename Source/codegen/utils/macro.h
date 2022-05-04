//
// Created by chen_ on 2022/5/3.
//
#include <type_traits>

#include "utils/Status.h"
#include "utils/logging.h"
#ifndef MYCC_SOURCE_CODEGEN_UTILS_MACROS_H_
#define MYCC_SOURCE_CODEGEN_UTILS_MACROS_H_
#define HZCC_ExceptOK_WITH_RETURN(expr)                                  \
    {                                                                    \
        auto macro_expand_ret_obj = std::move((expr));                   \
        static_assert(std::is_base_of_v<Hzcc::Status, decltype((expr))>, \
                      "Macro only supported type of Hzcc::Status");      \
                                                                         \
        if (!macro_expand_ret_obj.Ok()) {                                \
            VLOG(CODE_GEN_LEVEL) << macro_expand_ret_obj.Message();      \
            return std::move(macro_expand_ret_obj);                      \
        }                                                                \
    }

#endif  // MYCC_SOURCE_CODEGEN_UTILS_MACROS_H_
