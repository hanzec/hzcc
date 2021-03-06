//
// Created by chen_ on 2022/5/4.
//
#include "AST/statement/ReturnStmt.h"
#include "AST/statement/empty.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::ReturnStmt *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_LEAVE_RET_ON_STACK(
        HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetReturnVal())));

    if (!p_expr->GetReturnVal()->IsEmptyStmt()) {
        AddToCache(Utils::GetTypeName(p_expr->GetReturnVal()->RetType(), true) +
                   "return");
    } else {
        AddToCache("return");
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
