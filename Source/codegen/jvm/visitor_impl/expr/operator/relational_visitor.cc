//
// Created by chen_ on 2022/5/7.
//
#include "AST/expr/operator/RelationalExpr.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::RelationalExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // evaluate left expression
    HZCC_LEAVE_RET_ON_STACK(                                       // NOLINT
        HZCC_JVM_GENERATE_LOAD_INSTR(                              // NOLINT
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetLHS())));  // NOLINT

    // evaluate right expression
    HZCC_LEAVE_RET_ON_STACK(                                       // NOLINT
        HZCC_JVM_GENERATE_LOAD_INSTR(                              // NOLINT
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetRHS())));  // NOLINT

    /**
     * For relational operators that both LHS and RHS are integers(long short
     * char int), we can directly set the hint. Otherwise, we need to use the
     * dcmpg/dcmpl or fcmpg/fcmpl
     */
    if (p_expr->GetLHS()->RetType()->IsDouble() ||
        p_expr->GetLHS()->RetType()->IsFloat()) {
        std::string type_identifier =
            p_expr->GetLHS()->RetType()->IsDouble() ? "d" : "f";

        // geneate the instruction
        switch (p_expr->OpType()) {
            case AST::kRelationalType_Less:
                AddToCache(type_identifier + "cmpg");
                SetLastOpHint(OpHint::kOpHint_Logical_GreaterEqual);
                break;
            case AST::kRelationalType_Equal:
                AddToCache(type_identifier + "cmpl");
                SetLastOpHint(OpHint::kOpHint_Logical_NEqual);
                break;
            case AST::kRelationalType_NEqual:
                AddToCache(type_identifier + "cmpl");
                SetLastOpHint(OpHint::kOpHint_Logical_Equal);
                break;
            case AST::kRelationalType_Greater:
                AddToCache(type_identifier + "cmpl");
                SetLastOpHint(OpHint::kOpHint_Logical_LessEqual);
                break;
            case AST::kRelationalType_LessEqual:
                AddToCache(type_identifier + "cmpg");
                SetLastOpHint(OpHint::kOpHint_Logical_Greater);
                break;
            case AST::kRelationalType_GreaterEqual:
                AddToCache(type_identifier + "cmpl");
                SetLastOpHint(OpHint::kOpHint_Logical_Less);
                break;
            case AST::kRelationalType_ENUM_SIZE:
                DLOG(FATAL)
                    << "error Relational OP RetType : kRelationalType_ENUM_SIZE";
                break;
        }
    } else {
        SetLastOpHint(static_cast<OpHint>(p_expr->OpType()));
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
