//
// Created by chen_ on 2022/5/4.
//
#include "AST/expr/UnaryOperator.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/ConstValUtils.h"
namespace Hzcc::Codegen {
constexpr static const std::array<OpHint, OpHint::kOpHint_ENUM_SIZE>
    kOpHintReverseStrings = {kOpHint_GreaterEqual,
                             kOpHint_NEqual,
                             kOpHint_Equal,
                             kOpHint_LessEqual,
                             kOpHint_Greater,
                             kOpHint_Less,
                             kOpHint_Logical_GreaterEqual,
                             kOpHint_Logical_NEqual,
                             kOpHint_Logical_Equal,
                             kOpHint_Logical_LessEqual,
                             kOpHint_Logical_Greater,
                             kOpHint_Logical_Less,
                             kOpHint_Logical_NEqual};

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
    // TODO need to support all unary op
    switch (p_expr->GetUnaryType()) {
        case AST::kUnaryType_PreInc: {
            // visit node
            HZCC_JVM_NOT_GENERATE_LOAD_INSTR(
                HZCC_JVM_Visit_Node(p_expr->GetExpr()));

            auto var_name = ConsumeReturnStack();
            if (!IsGlobalVar(var_name) &&
                Utils::GetTypeName(p_expr->GetExpr()->RetType()) == "i") {
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " 1");
                if (_request_leave || _under_compare)
                    AddToCache(LoadFromVariable(var_name));
            } else {
                AddToCache(LoadFromVariable(var_name));
                AddToCache(Utils::PushConstVal(1));

                // Generate type convert instruction if necessary
                if (Utils::GetTypeName(p_expr->RetType(), true) != "i")
                    AddToCache("i2" +
                               Utils::GetTypeName(p_expr->RetType(), true));

                // increase and save
                AddToCache(Utils::GetTypeName(p_expr->RetType(), true) + "add");
                if (_request_leave || _under_compare) AddToCache("dup");
                AddToCache(SaveToVariable(var_name));
            }
        } break;
        case AST::kUnaryType_PreDec: {
            // visit node
            HZCC_JVM_NOT_GENERATE_LOAD_INSTR(
                HZCC_JVM_Visit_Node(p_expr->GetExpr()));

            auto var_name = ConsumeReturnStack();
            if (!IsGlobalVar(var_name) &&
                Utils::GetTypeName(p_expr->GetExpr()->RetType()) == "i") {
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " -1");
                if (_request_leave || _under_compare)
                    AddToCache(LoadFromVariable(var_name));
            } else {
                AddToCache(LoadFromVariable(var_name));
                AddToCache(Utils::PushConstVal(1));

                // Generate type convert instruction if necessary
                if (Utils::GetTypeName(p_expr->RetType(), true) != "i")
                    AddToCache("i2" +
                               Utils::GetTypeName(p_expr->RetType(), true));

                // increase and save
                AddToCache(Utils::GetTypeName(p_expr->RetType(), true) + "sub");
                if (_request_leave || _under_compare) AddToCache("dup");
                AddToCache(SaveToVariable(var_name));
            }
        } break;
        case AST::kUnaryType_PostInc: {
            // visit node
            HZCC_JVM_NOT_GENERATE_LOAD_INSTR(
                HZCC_JVM_Visit_Node(p_expr->GetExpr()));
            auto var_name = ConsumeReturnStack();
            if (!IsGlobalVar(var_name) &&
                Utils::GetTypeName(p_expr->GetExpr()->RetType()) == "i") {
                if (_request_leave || _under_compare)
                    AddToCache(LoadFromVariable(var_name));
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " 1");
            } else {
                AddToCache(LoadFromVariable(var_name));
                if (_request_leave || _under_compare) AddToCache("dup");
                AddToCache(Utils::PushConstVal(1));

                // Generate type convert instruction if necessary
                if (Utils::GetTypeName(p_expr->RetType(), true) != "i")
                    AddToCache("i2" +
                               Utils::GetTypeName(p_expr->RetType(), true));

                // increase and save
                AddToCache(Utils::GetTypeName(p_expr->RetType(), true) + "add");
                AddToCache(SaveToVariable(var_name));
            }
        } break;
        case AST::kUnaryType_PostDec: {
            // visit node
            HZCC_JVM_NOT_GENERATE_LOAD_INSTR(
                HZCC_JVM_Visit_Node(p_expr->GetExpr()));

            auto var_name = ConsumeReturnStack();
            if (!IsGlobalVar(var_name) &&
                Utils::GetTypeName(p_expr->GetExpr()->RetType()) == "i") {
                if (_request_leave || _under_compare)
                    AddToCache(LoadFromVariable(var_name));
                AddToCache("iinc " + std::to_string(GetStackID(var_name)) +
                           " -1");
            } else {
                AddToCache(LoadFromVariable(var_name));
                if (_request_leave || _under_compare) AddToCache("dup");
                AddToCache(Utils::PushConstVal(1));

                // Generate type convert instruction if necessary
                if (Utils::GetTypeName(p_expr->RetType(), true) != "i")
                    AddToCache("i2" +
                               Utils::GetTypeName(p_expr->RetType(), true));

                // increase and save
                AddToCache(Utils::GetTypeName(p_expr->RetType(), true) + "sub");
                AddToCache(SaveToVariable(var_name));
            }
        } break;
        case AST::kUnaryType_UnaryMinus:
            if (_request_leave || _under_compare) {
                auto last_op_hint = GetLastOpHint();
                if (last_op_hint == kOpHint_None) {
                    HZCC_JVM_GENERATE_LOAD_INSTR(
                        HZCC_JVM_Visit_Node(p_expr->GetExpr()));
                    AddToCache(Utils::GetTypeName(p_expr->RetType(), true) +
                               "neg");
                } else {
                    SetLastOpHint(kOpHintReverseStrings[last_op_hint]);
                }
            }
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
