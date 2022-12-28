//
// Created by chen_ on 2022/5/7.
//
#include "AST/statement/IfStmt.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintReversedStrings = {
        "if_icmpge ", "if_icmpne ", "if_icmpeq ", "if_icmple ", "if_icmpgt ",
        "if_icmplt ", "ifge ",      "ifne ",      "ifeq ",      "ifle ",
        "ifgt",       "iflt ",      "ifeq "};

Status JVMGenerator::visit(Hzcc::AST::IfStmt *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    /**
     * The format of the if-elseif-else stmt is:
     *      ...
     *      ...IfCondition...
     *      Goto Label_Exit if condition is false
     *  Label_if_entry:
     *      ...IfBody...
     *      Goto Label_Exit_ALL;
     *  Label_Exit:
     *      ...ElseIf(0)Condition...
     *      Goto Label_elseif_0_exit if condition is false
     *  Label_elseif_0_entry:
     *      ...ElseIf(1)Body...
     *      Goto Label_Exit_ALL
     *  Label_elseif_0_exit:
     *      ...ElseIf(1)Condition...
     *      Goto Label_elseif_1_exit if condition is false
     *  Label_elseif_1_entry:
     *      ...ElseIfBody...
     *      Goto Label_Exit_ALL
     *      ...
     *      ...
     *  Label_elseif_n_exit:
     *      ...ElseBody...
     *  Label_Exit_ALL:
     *  If we have the else body, after all the elseif conditions are false, we
     * will jump to the else body. Otherwise, we will directly exit the if
     * stmt since the Label_Exit is the end of the if stmt.
     *
     */

    EnterScope(p_expr->Location(), kScopedType_If);
    auto if_exit_label = GetScopeExitLabel();
    auto if_exit_all_label = GenerateLabel("stmt_end");

    // Generate the code for the conditional .
    HZCC_CONTEXT_COMPARE(HZCC_JVM_Visit_Node(p_expr->CondStmt()))
    AddToCache(kOpHintReversedStrings[GetLastOpHint()] + if_exit_label);

    // add begin label and generate the code for the if statements.
    DecLindeIndent();
    AddToCache(GetScopeEntryLabel() + ":");
    IncLindeIndent();

    HZCC_NOT_LEAVE_RET_ON_STACK(                                    // NOLINT
        HZCC_CONTEXT_LEAVE_COMPARE(                                 // NOLINT
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->BodyStmt())))  // NOLINT
    AddToCache("goto " + if_exit_all_label);

    // finally place the exit label.
    DecLindeIndent();
    AddToCache(if_exit_label + ":");
    IncLindeIndent();

    // Generate the code for else-if statements.
    for (auto &else_if_stmt : p_expr->ElseIfStmt()) {
        EnterScope(else_if_stmt.first->Location(), kScopedType_Else_If);

        // generate else-if cond code.
        HZCC_CONTEXT_COMPARE(HZCC_JVM_Visit_Node(else_if_stmt.first))
        AddToCache(kOpHintReversedStrings[GetLastOpHint()] +
                   GetScopeExitLabel());

        DecLindeIndent();
        AddToCache(GetScopeEntryLabel() + ":");
        IncLindeIndent();

        // generate else-if body code.
        HZCC_NOT_LEAVE_RET_ON_STACK(     // NOLINT
            HZCC_CONTEXT_LEAVE_COMPARE(  // NOLINT
                HZCC_JVM_Use_Deduced_IF_POSSIBLE(else_if_stmt.second)))
        AddToCache("goto " + if_exit_all_label);

        DecLindeIndent();
        AddToCache(GetScopeExitLabel() + ":");
        IncLindeIndent();

        LeaveScope();
    }

    // generate the else stmt if it has one
    if (p_expr->HasElse()) {
        HZCC_NOT_LEAVE_RET_ON_STACK(     // NOLINT
            HZCC_CONTEXT_LEAVE_COMPARE(  // NOLINT
                HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->ElseStmt())))
    }

    // finally place the if body exit label.
    DecLindeIndent();
    AddToCache(if_exit_all_label + ":");
    IncLindeIndent();

    LeaveScope();
    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
