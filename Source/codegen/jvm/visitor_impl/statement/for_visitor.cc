//
// Created by chen_ on 2022/5/6.
//
#include "AST/statement/for.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::ForStatement *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << " p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    EnterScope(p_expr->Location(), kScopedType_For);
    auto exit_scope = GetScopeExitLabel();

    LeaveScope();
    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen