//
// Created by chen_ on 2022/5/4.
//
#include "AST/statement/empty.h"
#include "AST/statement/return.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::ReturnNode *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    HZCC_JVM_REQUEST_LEAVE_VAL(
        HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetReturnVal()));

    if (typeid(p_expr->GetReturnVal()) != typeid(AST::EmptyStatement)) {
        AddToCache(Utils::GetTypeName(p_expr->GetReturnVal()->GetType(), true) +
                   "return");
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen