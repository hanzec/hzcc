//
// Created by chen_ on 2022/5/5.
//
#include "AST/expr/cast.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::CastExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(p_expr->GetType()) << "p_expr have nullptr type";
    DLOG_ASSERT(p_expr->GetCastExpr()) << "p_expr have nullptr expr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // firs we get the type of the expr
    HZCC_JVM_Visit_Node(p_expr->GetCastExpr());

    // then we get the type of the cast
    if (_generate_load) {
        // avoid generate instruction like "i2i, "c2i"
        if (Utils::GetTypeName(p_expr->GetCastExpr()->GetType(), true) !=
            Utils::GetTypeName(p_expr->GetType())) {
            AddToCache(
                Utils::GetTypeName(p_expr->GetCastExpr()->GetType(), true) +
                "2" + Utils::GetTypeName(p_expr->GetType()));
        }
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen