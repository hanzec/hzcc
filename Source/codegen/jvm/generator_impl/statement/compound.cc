//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/compound.h"

#include "codegen/jvm/JVMGenerator.h"
namespace Mycc::Codegen {

void JVMGenerator::visit(Mycc::AST::CompoundStmt *p_expr) {
    IncLindeIndent();

    // generate the body
    auto &file_handler = GetOstream();
    for (const auto &stmt : p_expr->GetBodyStatements()) {
        // first we need to print annotations
        file_handler << GetLineIndent() << ";; "
                     << (stmt->IsReturn() ? "return " : "expression ")
                     << GetInputFile() << " " << stmt->GetLine() << "\n";

        // then we generate the statement
        stmt->visit(*this);
    }

    DecLindeIndent();
}
}  // namespace Mycc::Codegen
