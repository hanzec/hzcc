//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/CompoundStmt.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::CompoundStmt *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    for (const auto &stmt : p_expr->GetBodyStatements()) {
        // first we need to print annotations
        std::stringstream ss;
        ss << ";; " << (stmt->IsReturn() ? "return " : "expression ")
           << GetInputFileName() << " " << stmt->GetLine();
        AddToCache(ss.str());

        // also line number
        DecLindeIndent();
        auto label = GenerateLineLabel();
        AddToCache(label + ":");
        SetLineNumberTag(label, stmt->GetLine());
        IncLindeIndent();

        // then we generate the stmt
        HZCC_NOT_LEAVE_RET_ON_STACK(
            HZCC_CONTEXT_LEAVE_COMPARE(HZCC_JVM_Visit_Node(stmt)));
    }
    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
