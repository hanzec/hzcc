//
// Created by chen_ on 2022/5/6.
//
#include "AST/statement/for.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintReverseStrings = {
        "if_icmpge ", "if_icmpne ", "if_icmpeq ", "if_icmple ", "if_icmpgt ",
        "if_icmplt ", "ifge ",      "ifne ",      "ifeq ",      "ifle ",
        "ifgt",       "iflt ",      "ifeq "};

Status JVMGenerator::visit(Hzcc::AST::ForStatement *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << " p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    /**
     * The design of for statement is:
     *      ...
     *      ...Init...
     *   Label_cond:
     *      ...Condition...
     *      if not true goto Label_end
     *      ...Body...
     *   Label_Loop_begin:
     *      ...Increment...
     *      goto Label_cond
     *   Label_Loop_end:
     *      ...
     *
     *   This kind of design will make continue statement much easier. Since
     * each continue statement will jump to the beginning of the loop which need
     * call the increment statement
     */

    // Generate the code for the initialization statement.
    HZCC_NOT_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->InitStmt()))

    EnterScope(p_expr->Location(), kScopedType_For);

    // helper label for the loop condition.
    DecLindeIndent();
    auto cond_label = GenerateLabel("for_cond");
    AddToCache(cond_label + ":");
    IncLindeIndent();

    // generate the condition.
    if (!p_expr->CondStmt()->IsEmptyStmt()) {
        HZCC_CONTEXT_COMPARE(HZCC_JVM_Visit_Node(p_expr->CondStmt()))
        AddToCache(kOpHintReverseStrings[GetLastOpHint()] +
                   GetScopeExitLabel());
    }

    // generate the function body.
    HZCC_CONTEXT_LEAVE_COMPARE(                        // NOLINT
        HZCC_NOT_LEAVE_RET_ON_STACK(                   // NOLINT
            HZCC_JVM_Visit_Node(p_expr->BodyStmt())))  // NOLINT

    // Generate the code for the update statement.
    DecLindeIndent();
    AddToCache(GetScopeEntryLabel() + ":");
    IncLindeIndent();

    // generate the update statement.
    if (!p_expr->StepStmt()->IsEmptyStmt()) {
        HZCC_CONTEXT_LEAVE_COMPARE(                        // NOLINT
            HZCC_NOT_LEAVE_RET_ON_STACK(                   // NOLINT
                HZCC_JVM_Visit_Node(p_expr->StepStmt())))  // NOLINT
    }

    // add goto to the beginning of the loop.
    AddToCache("goto " + cond_label);

    // loop exit label.
    DecLindeIndent();
    AddToCache(GetScopeExitLabel() + ":");
    IncLindeIndent();

    // generate the loop exit label.
    LeaveScope();
    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen