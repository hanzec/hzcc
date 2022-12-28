//
// Created by chen_ on 2022/5/5.
//
#include "AST/expr/CastExpr.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::CastExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(p_expr->RetType()) << "p_expr have nullptr type";
    DLOG_ASSERT(p_expr->GetCastExpr()) << "p_expr have nullptr cast";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // firs we get the type of the cast
    HZCC_JVM_Visit_Node(p_expr->GetCastExpr());

    // then we get the type of the cast
    if (_generate_load) {
        // avoid generate instruction like "i2i, "c2i"
        if (Utils::GetTypeName(p_expr->GetCastExpr()->RetType(), true) !=
            Utils::GetTypeName(p_expr->RetType())) {
            AddToCache(
                Utils::GetTypeName(p_expr->GetCastExpr()->RetType(), true) +
                "2" + Utils::GetTypeName(p_expr->RetType()));
        }
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
