//
// Created by chen_ on 2022/5/7.
//
#include "AST/statement/do.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintStrings = {"if_icmplt ", "if_icmpeq ", "if_icmpne ", "if_icmpgt ",
                      "if_icmple ", "if_icmpge ", "iflt ",      "ifeq ",
                      "ifne ",      "ifgt ",      "ifle",       "ifge ",
                      "ifne "};

Status JVMGenerator::visit(Hzcc::AST::DoStatement *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    EnterScope(p_expr->Location(), kScopedType_Do);

    // we place the entry label first
    DecLindeIndent();
    AddToCache(GetScopeEntryLabel() + ":");
    IncLindeIndent();

    // then we generate the body
    HZCC_CONTEXT_LEAVE_COMPARE(       // NOLINT
        HZCC_NOT_LEAVE_RET_ON_STACK(  // NOLINT
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->BodyStmt())))

    // then we generate the condition
    if (!p_expr->CondStmt()->IsEmptyStmt()) {
        HZCC_CONTEXT_COMPARE(                              // NOLINT
            HZCC_NOT_LEAVE_RET_ON_STACK(                   // NOLINT
                HZCC_JVM_Visit_Node(p_expr->CondStmt())))  // NOLINT
        AddToCache(kOpHintStrings[GetLastOpHint()] + GetScopeEntryLabel());
    } else {
        AddToCache("goto " + GetScopeEntryLabel());
    }

    // write exit label
    DecLindeIndent();
    AddToCache(GetScopeExitLabel() + ":");
    IncLindeIndent();

    LeaveScope();
    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen