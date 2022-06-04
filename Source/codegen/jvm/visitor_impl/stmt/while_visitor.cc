//
// Created by chen_ on 2022/5/7.
//
#include "AST/statement/WhileStmt.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintReversedStrings = {
        "if_icmpge ", "if_icmpne ", "if_icmpeq ", "if_icmple ", "if_icmpgt ",
        "if_icmplt ", "ifge ",      "ifne ",      "ifeq ",      "ifle ",
        "ifgt",       "iflt ",      "ifeq "};

Status JVMGenerator::visit(Hzcc::AST::WhileStmt *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    EnterScope(p_expr->Location(), kScopedType_While);

    // we place the entry label first
    DecLindeIndent();
    AddToCache(GetScopeEntryLabel() + ":");
    IncLindeIndent();

    // if condition is flase, jump to exit label
    HZCC_CONTEXT_COMPARE(HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->CondStmt()))
    AddToCache(kOpHintReversedStrings[GetLastOpHint()] + GetScopeExitLabel());

    // then we generate the body
    HZCC_NOT_LEAVE_RET_ON_STACK(
        HZCC_CONTEXT_LEAVE_COMPARE(HZCC_JVM_Visit_Node(p_expr->BodyStmt())))
    AddToCache("goto " + GetScopeEntryLabel());

    // write exit label
    DecLindeIndent();
    AddToCache(GetScopeExitLabel() + ":");
    IncLindeIndent();

    LeaveScope();

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
