//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/CompoundStmt.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

/**
 * @brief Emit JVM assembly for a CompoundStmt node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
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
