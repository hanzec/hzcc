//
// Created by chen_ on 2022/5/8.
//
#include "AST/expr/conditional.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
constexpr static const std::array<const char *, OpHint::kOpHint_ENUM_SIZE>
    kOpHintStrings = {"if_icmpge ", "if_icmpne ", "if_icmpeq ", "if_icmple ",
                      "if_icmpgt ", "if_icmplt ", "ifge ",      "ifne ",
                      "ifeq ",      "ifle ",      "ifgt",       "iflt ",
                      "ifeq "};

Status JVMGenerator::visit(Hzcc::AST::ConditionalExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    EnterScope(p_expr->Location(), kScopedType_Ternary);

    /**
     * Ternary operator structure:
     *     ...
     *     ...Condition...
     *  LABEL_entry:
     *     ...TrueStatement...
     *     goto LABEL_exit;
     *  Label_exit:
     *     ...FalseStatement...
     */

    // Generate condition expression
    HZCC_CONTEXT_COMPARE(HZCC_JVM_Visit_Node(p_expr->GetContStmt()))
    AddToCache(kOpHintStrings[GetLastOpHint()] + GetScopeExitLabel());

    DecLindeIndent();
    AddToCache(GetScopeEntryLabel() + ":");
    IncLindeIndent();

    HZCC_NOT_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->GetTrueExpr()));
    AddToCache("goto " + GetScopeExitLabel());

    DecLindeIndent();
    AddToCache(GetScopeExitLabel() + ":");
    IncLindeIndent();

    HZCC_NOT_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->GetFalseExpr()));

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen