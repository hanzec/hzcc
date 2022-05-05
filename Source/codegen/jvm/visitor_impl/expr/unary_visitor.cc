//
// Created by chen_ on 2022/5/4.
//
#include "AST/expr/unary.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/ConstValUtils.h"
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
    // Generate code for the expression
    HZCC_JVM_NOT_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetExpr()));

    // TODO need to support all unary operator
    auto var_name = ConsumeReturnStack();
    switch (p_expr->GetUnaryType()) {
        case AST::kUnaryType_PreInc:
            if (Utils::GetTypeName(p_expr->GetExpr()->GetType()) == "i") {
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " 1");
                AddToCache(LoadFromVariable(var_name));
            } else {
                AddToCache(LoadFromVariable(var_name));
                AddToCache(Utils::PushConstVal(1));
                AddToCache(Utils::GetTypeName(p_expr->GetType(), true) + "add");
                AddToCache("dup");
                AddToCache(SaveToVariable(var_name));
            }
            break;
        case AST::kUnaryType_PreDec:
            if (Utils::GetTypeName(p_expr->GetExpr()->GetType()) == "i") {
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " -1");
                AddToCache(LoadFromVariable(var_name));
            } else {
                AddToCache(LoadFromVariable(var_name));
                AddToCache(Utils::PushConstVal(1));
                AddToCache(Utils::GetTypeName(p_expr->GetType(), true) + "sub");
                AddToCache("dup");
                AddToCache(SaveToVariable(var_name));
            }
            break;
        case AST::kUnaryType_PostInc:
            if (Utils::GetTypeName(p_expr->GetExpr()->GetType()) == "i") {
                AddToCache(LoadFromVariable(var_name));
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " 1");
            } else {
                AddToCache(LoadFromVariable(var_name));
                AddToCache("dup");
                AddToCache(Utils::PushConstVal(1));
                AddToCache(Utils::GetTypeName(p_expr->GetType(), true) + "add");
                AddToCache(SaveToVariable(var_name));
            }
            break;
        case AST::kUnaryType_PostDec:
            if (Utils::GetTypeName(p_expr->GetExpr()->GetType()) == "i") {
                AddToCache(LoadFromVariable(var_name));
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " -1");
            } else {
                AddToCache(LoadFromVariable(var_name));
                AddToCache("dup");
                AddToCache(Utils::PushConstVal(1));
                AddToCache(Utils::GetTypeName(p_expr->GetType(), true) + "sub");
                AddToCache(SaveToVariable(var_name));
            }
            break;
        case AST::kUnaryType_UnaryMinus:
            AddToCache(LoadFromVariable(var_name));
            AddToCache("ineg");
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