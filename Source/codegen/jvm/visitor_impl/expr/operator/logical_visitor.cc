//
// Created by chen_ on 2022/5/7.
//
#include "AST/expr/operator/logical.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintStrings = {"if_icmplt ", "if_icmpeq ", "if_icmpne ", "if_icmpgt ",
                      "if_icmple ", "if_icmpge ", "iflt ",      "ifeq ",
                      "ifne ",      "ifgt ",      "ifle",       "ifge ",
                      "ifne "};

constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintReversedStrings = {
        "if_icmpge ", "if_icmpne ", "if_icmpeq ", "if_icmple ", "if_icmpgt ",
        "if_icmplt ", "ifge ",      "ifne ",      "ifeq ",      "ifle ",
        "ifgt",       "iflt ",      "ifeq "};

Status JVMGenerator::visit(Hzcc::AST::LogicalExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // evaluate left expression
    HZCC_CONTEXT_COMPARE(                             // NOLINT
        HZCC_NOT_LEAVE_RET_ON_STACK(                  // NOLINT
            HZCC_JVM_Visit_Node(p_expr->GetLHS())));  // NOLINT

    if (!_under_compare) {
        EnterScope(p_expr->Location(), kScopedType_Logical);

        auto label = GenerateLabel("assign_value");

        /**
         * ASM structure for assign value to variable:
         *     ...
         *     ...LHS...
         *     if LHS's value is false then goto Label
         *     ...RHS..
         *     if RHS's value is false then goto Label
         *     goto Label_exit
         * Label:
         *      ICONST_0 (since value is false)
         * Label_exit:
         *     ...
         */
        if (p_expr->GetLogicalType() == AST::kLogicalType_And) {
            // if LHS is false then goto Label
            AddToCache(kOpHintReversedStrings[GetLastOpHint()] + label);

            // evaluate right expression
            HZCC_CONTEXT_COMPARE(                             // NOLINT
                HZCC_NOT_LEAVE_RET_ON_STACK(                  // NOLINT
                    HZCC_JVM_Visit_Node(p_expr->GetRHS())));  // NOLINT

            // if RHS is false then goto Label
            AddToCache(kOpHintReversedStrings[GetLastOpHint()] + label);
            AddToCache("goto " + GetScopeExitLabel());

            // Label
            DecLindeIndent();
            AddToCache(label + ":");
            IncLindeIndent();

            // ICONST_0
            AddToCache("iconst_0");

            // Label_exit
            AddToCache(GetScopeExitLabel() + ":");
        }

        /**
         * ASM structure for assign value to variable:
         *     ...
         *     ...LHS...
         *     if LHS's value is true then goto Label
         *     ...RHS..
         *     if RHS's value is true then goto Label
         *     goto Label_exit
         * Label:
         *      ICONST_1 (since value is true)
         * Label_exit:
         *     ...
         */
        else {
            // if LHS is false then goto Label
            AddToCache(kOpHintStrings[GetLastOpHint()] + label);

            // evaluate right expression
            HZCC_NOT_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->GetRHS()));

            // if RHS is false then goto Label
            AddToCache(kOpHintStrings[GetLastOpHint()] + label);
            AddToCache("goto " + GetScopeExitLabel());

            // Label
            DecLindeIndent();
            AddToCache(label + ":");
            IncLindeIndent();

            // ICONST_0
            AddToCache("iconst_1");
        }

        // Label_exit
        AddToCache(GetScopeExitLabel() + ":");
        LeaveScope();
    }

    // logical circuit is not needed
    else {
        if (p_expr->GetLogicalType() == AST::kLogicalType_And) {
            // if LHS is false then goto scope exit Label
            AddToCache(kOpHintReversedStrings[GetLastOpHint()] +
                       GetScopeExitLabel());
        } else {
            // if LHS is true then goto scope exit Label
            AddToCache(kOpHintStrings[GetLastOpHint()] + GetScopeEntryLabel());
        }

        // evaluate right expression
        HZCC_NOT_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->GetRHS()));
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen