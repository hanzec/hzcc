//
// Created by chen_ on 2022/5/5.
//
#include "AST/statement/function_call.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::FunctionCall* p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    // generate ars push
    for (auto& arg : p_expr->GetArgsNode()) {
        HZCC_LEAVE_RET_ON_STACK(HZCC_JVM_GENERATE_LOAD_INSTR(
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(arg)));
    }

    // generate function call
    if (_function_table.find(p_expr->FuncName()) != _function_table.end()) {
        auto [class_name, function_signature] =
            _function_table[p_expr->FuncName()];
        AddToCache("invokestatic Method " + class_name + " " +
                   p_expr->FuncName() + " " + function_signature);

        // pop if does not need return value
        if (!_request_leave && !p_expr->GetType()->IsVoid()) {
            AddToCache("pop");
        }
    } else {
        DLOG(WARNING) << "Function " << p_expr->FuncName()
                      << " is not defined, skip generation";
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
