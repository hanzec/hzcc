//
// Created by chen_ on 2022/5/4.
//
#include "AST/expr/unary.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::UnaryExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(p_expr->GetExpr() != nullptr) << "p_expr->GetExpr() is nullptr";
    DLOG_ASSERT(p_expr->GetUnaryType() != AST::kUnaryType_ENUM_SIZE)
        << "p_expr->GetUnaryType() is kUnaryType_ENUM_SIZE";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // TODO need to support all unary operator
    switch (p_expr->GetUnaryType()) {
        case AST::kUnaryType_PreInc:
            break;
        case AST::kUnaryType_PreDec:
            break;
        case AST::kUnaryType_PostInc:
            break;
        case AST::kUnaryType_PostDec:
            break;
        case AST::kUnaryType_UnaryMinus:
            break;
        case AST::kUnaryType_Reference:
        case AST::kUnaryType_Dereference:
        case AST::kUnaryType_LogicalNot:
        case AST::kUnaryType_BitwiseNot:
        case AST::kUnaryType_ENUM_SIZE:
            DLOG(FATAL) << "Unsupported UnaryType";
            break;
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen