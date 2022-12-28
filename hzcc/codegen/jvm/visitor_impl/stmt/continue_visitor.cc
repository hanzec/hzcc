//
// Created by chen_ on 2022/5/7.
//
#include "AST/statement/empty.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

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
