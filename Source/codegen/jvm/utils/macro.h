//
// Created by chen_ on 2022/5/3.
//
#include "AST/DeduceValue.h"
#include "AST/type/Type.h"
#include "ConstValUtils.h"
#ifndef MYCC_SOURCE_CODEGEN_JVM_UTILS_MACRO_H_
#define MYCC_SOURCE_CODEGEN_JVM_UTILS_MACRO_H_
#define HZCC_JVM_GET_EXPR_TYPE(expr) \
    std::string(1,                   \
                Utils::GetJVMTypename(p_expr->GetLHS()->GetType()->GetName()))

#define HZCC_JVM_GENERATE_LOAD_INSTR(expr)            \
    {                                                 \
        auto prev_status = GetGenerateLoadStatus();   \
        if (!prev_status) void EnableGenerateLoad();  \
        {expr};                                       \
        if (!prev_status) void DisableGenerateLoad(); \
    }

#define HZCC_JVM_NOT_GENERATE_LOAD_INSTR(expr)       \
    {                                                \
        auto prev_status = GetGenerateLoadStatus();  \
        if (prev_status) void DisableGenerateLoad(); \
        {expr};                                      \
        if (prev_status) void EnableGenerateLoad();  \
    }

#define HZCC_JVM_Visit_Node(node)                                          \
    static_assert(                                                         \
        std::is_convertible<                                               \
            decltype((node)),                                              \
            const std::unique_ptr<Hzcc::AST::ASTNode>&>::value,            \
        "Macro type mismatch, need std::unique_ptr<Hzcc::AST::ASTNode>&"); \
    DLOG_ASSERT((node) != nullptr) << "Failed to visit node because of "   \
                                      "nullptr";                           \
    DVLOG(CODE_GEN_LEVEL) << "Use expr: " << #node << "visit ASTNode "     \
                          << (node)->GetNodeName()                         \
                          << "(line: " << (node)->GetLine() << ")";        \
    HZCC_ExceptOK_WITH_RETURN((node)->visit(*this));

#define HZCC_JVM_Use_Deduced_IF_POSSIBLE(expr)                                 \
    {                                                                          \
        static_assert(                                                         \
            std::is_convertible<                                               \
                decltype((expr)),                                              \
                const std::unique_ptr<Hzcc::AST::ASTNode>&>::value,            \
            "Macro type mismatch, need std::unique_ptr<Hzcc::AST::ASTNode>&"); \
                                                                               \
        const auto& deduced_lhs = (expr)->GetDeducedValue();                   \
        if (deduced_lhs.has_value()) {                                         \
            AddToCache(Utils::PushConstVal((expr)->GetType()->GetName(),       \
                                           deduced_lhs.value()));              \
            DVLOG(CODE_GEN_LEVEL)                                              \
                << "use deduced value of " << (expr)->GetNodeName()            \
                << "(line: " << (expr)->GetLine() << ")";                      \
        } else {                                                               \
            HZCC_JVM_Visit_Node(expr);                                         \
        }                                                                      \
    }

#endif  // MYCC_SOURCE_CODEGEN_JVM_UTILS_MACRO_H_
