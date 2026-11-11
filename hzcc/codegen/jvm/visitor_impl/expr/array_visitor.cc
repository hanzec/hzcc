//
// Created by chen_ on 2022/5/4.
//
#include "AST/expr/ArraySubscriptExpr.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
/**
 * @brief Emit JVM assembly for a ArraySubscriptExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status JVMGenerator::visit(Hzcc::AST::ArraySubscriptExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(p_expr->GetArrayBase() != nullptr)
        << "p_expr->GetArrayBase() is nullptr";
    DLOG_ASSERT(p_expr->GetSubscript() != nullptr)
        << "p_expr->GetSubscript() is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    // first visit DeclRefExpr
    HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetArrayBase()));

    // load array base
    HZCC_LEAVE_RET_ON_STACK(                   // NOLINT
        HZCC_JVM_GENERATE_LOAD_INSTR(          // NOLINT
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(  // NOLINT
                p_expr->GetSubscript())));     // NOLINT

    // generate load instr
    if (_generate_load || _under_compare) {
        AddToCache(Utils::GetTypeName(p_expr->RetType()) + "aload");
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
