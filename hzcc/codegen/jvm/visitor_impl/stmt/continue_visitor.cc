//
// Created by chen_ on 2022/5/7.
//
#include "AST/statement/empty.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

/**
 * @brief Emit JVM assembly for a ContinueStmt node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status JVMGenerator::visit(Hzcc::AST::ContinueStmt *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(WithinLoop()) << "break stmt not called within in loop";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // Jump to the start of the loop
    AddToCache("goto " + GetLoopEntryLabel());

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
