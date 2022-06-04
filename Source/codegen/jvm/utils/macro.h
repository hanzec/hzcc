//
// Created by chen_ on 2022/5/3.
//
#include <cctype>

#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "ConstValUtils.h"
#include "TypeUtils.h"
#include "utils/logging.h"
#ifndef HZCC_CODEGEN_JVM_UTILS_MACRO_H
#define HZCC_CODEGEN_JVM_UTILS_MACRO_H

#define HZCC_LEAVE_RET_ON_STACK(expr)                       \
    {                                                       \
        const auto leave_val_prev_status = _request_leave;  \
        if (!leave_val_prev_status) _request_leave = true;  \
        {expr};                                             \
        if (!leave_val_prev_status) _request_leave = false; \
    }

#define HZCC_NOT_LEAVE_RET_ON_STACK(expr)                  \
    {                                                      \
        auto leave_val_prev_status = _request_leave;       \
        if (leave_val_prev_status) _request_leave = false; \
        {expr};                                            \
        if (leave_val_prev_status) _request_leave = true;  \
    }

#define HZCC_CONTEXT_COMPARE(expr)                                 \
    {                                                              \
        const auto need_compare_ret_prev_status = _under_compare;  \
        if (!need_compare_ret_prev_status) _under_compare = true;  \
        {expr};                                                    \
        if (!need_compare_ret_prev_status) _under_compare = false; \
    }

#define HZCC_CONTEXT_LEAVE_COMPARE(expr)                          \
    {                                                             \
        auto need_compare_ret_prev_status = _under_compare;       \
        if (need_compare_ret_prev_status) _under_compare = false; \
        {expr};                                                   \
        if (need_compare_ret_prev_status) _under_compare = true;  \
    }

#define HZCC_JVM_GENERATE_LOAD_INSTR(expr)                   \
    {                                                        \
        const auto load_instr_prev_status = _generate_load;  \
        if (!load_instr_prev_status) _generate_load = true;  \
        {expr};                                              \
        if (!load_instr_prev_status) _generate_load = false; \
    }

#define HZCC_JVM_NOT_GENERATE_LOAD_INSTR(expr)              \
    {                                                       \
        auto load_instr_prev_status = _generate_load;       \
        if (load_instr_prev_status) _generate_load = false; \
        {expr};                                             \
        if (load_instr_prev_status) _generate_load = true;  \
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
            if (_request_leave) {                                              \
                AddToCache(Utils::PushConstVal((expr)->GetType()->GetName(),   \
                                               deduced_lhs.value()));          \
                DVLOG(MESSAGE_ERROR_TRACING)                                   \
                    << "use deduced value of " << (expr)->NodeName()           \
                    << "(line: " << (expr)->GetLine() << ")";                  \
            } else {                                                           \
                DVLOG(MESSAGE_ERROR_TRACING)                                   \
                    << "skip code gen for node " << p_expr->NodeName()         \
                    << "(line:" << p_expr->GetLine()                           \
                    << " id: " << p_expr->GetNodeId() << ")";                  \
            }                                                                  \
        } else {                                                               \
            HZCC_JVM_Visit_Node(expr);                                         \
        }                                                                      \
    }

#endif  // HZCC_CODEGEN_JVM_UTILS_MACRO_H
