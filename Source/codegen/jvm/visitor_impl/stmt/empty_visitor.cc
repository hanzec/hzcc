//
// Created by chen_ on 2022/5/4.
//
#include "AST/statement/empty.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::EmptyStatement *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    // EmptyStatement do nothing

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
