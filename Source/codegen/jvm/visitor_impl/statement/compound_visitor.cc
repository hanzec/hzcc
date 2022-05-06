//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/compound.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {

Status JVMGenerator::visit(Hzcc::AST::CompoundStmt *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    IncLindeIndent();
    for (const auto &stmt : p_expr->GetBodyStatements()) {
        // first we need to print annotations
        std::stringstream ss;
        ss << ";; " << (stmt->IsReturn() ? "return " : "expression ")
           << GetInputFileName() << " " << stmt->GetLine();
        AddToCache(ss.str());

        // then we generate the statement
        HZCC_JVM_Visit_Node(stmt);

        // generate return statement if last statement is not return
        if (stmt == p_expr->GetBodyStatements().back() && !stmt->IsReturn()) {
            AddToCache("return");
        }
    }
    DecLindeIndent();
    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
