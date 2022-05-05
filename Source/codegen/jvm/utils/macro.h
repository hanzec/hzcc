//
// Created by chen_ on 2022/5/3.
//
#include <cctype>

#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "ConstValUtils.h"
#include "TypeUtils.h"
#include "utils/logging.h"
#ifndef MYCC_SOURCE_CODEGEN_JVM_UTILS_MACRO_H_
#define MYCC_SOURCE_CODEGEN_JVM_UTILS_MACRO_H_
#define HZCC_JVM_GENERATE_LOAD_INSTR(expr)        \
    {                                             \
        const auto prev_status = _generate_load;  \
        if (!prev_status) _generate_load = true;  \
        {expr};                                   \
        if (!prev_status) _generate_load = false; \
    }

#define HZCC_JVM_NOT_GENERATE_LOAD_INSTR(expr)   \
    {                                            \
        auto prev_status = _generate_load;       \
        if (prev_status) _generate_load = false; \
        {expr};                                  \
        if (prev_status) _generate_load = true;  \
    }

#define HZCC_JVM_REQUEST_LEAVE_VAL(expr)             \
    {                                                \
        const auto prev_status = _request_leave;     \
        if (!_request_leave) _request_leave = true;  \
        {expr};                                      \
        if (!_request_leave) _request_leave = false; \
    }

#define HZCC_JVM_NOT_REQUEST_LEAVE_VAL(expr)        \
    {                                               \
        auto prev_status = _request_leave;          \
        if (_request_leave) _request_leave = false; \
        {expr};                                     \
        if (_request_leave) _request_leave = true;  \
    }

#define HZCC_JVM_Visit_Node(node)                                          \
    static_assert(                                                         \
        std::is_convertible<decltype((node.get())),                        \
                            const Hzcc::AST::ASTNode*>::value,             \
        "Macro type mismatch, need std::unique_ptr<Hzcc::AST::ASTNode>&"); \
    DLOG_ASSERT((node) != nullptr) << "Failed to visit node because of "   \
                                      "nullptr";                           \
    HZCC_ExceptOK_WITH_RETURN((node)->visit(*this));

#define HZCC_JVM_Use_Deduced_IF_POSSIBLE(expr)                                 \
    {                                                                          \
        static_assert(                                                         \
            std::is_convertible<decltype((expr.get())),                        \
                                const Hzcc::AST::ASTNode*>::value,             \
            "Macro type mismatch, need std::unique_ptr<Hzcc::AST::ASTNode>&"); \
                                                                               \
        const auto& deduced_lhs = (expr)->GetDeducedValue();                   \
        if (deduced_lhs.has_value()) {                                         \
            AddToCache(Utils::PushConstVal((expr)->GetType()->GetName(),       \
                                           deduced_lhs.value()));              \
            DVLOG(MESSAGE_ERROR_TRACING)                                       \
                << "use deduced value of " << (expr)->GetNodeName()            \
                << "(line: " << (expr)->GetLine() << ")";                      \
        } else {                                                               \
            HZCC_JVM_Visit_Node(expr);                                         \
        }                                                                      \
    }

#endif  // MYCC_SOURCE_CODEGEN_JVM_UTILS_MACRO_H_
