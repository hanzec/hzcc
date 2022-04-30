//
// Created by hanzech on 22-4-29.
//
#include "codegen/jvm/JVMGenerator.h"
#include "AST/CompilationUnit.h"
namespace Mycc::Codegen {
Status JVMGenerator::VisitAllAST(
    const std::unique_ptr<Mycc::AST::CompilationUnit> &p_expr) {
    // handle init section
    for (auto &decl : p_expr->GetDecls()){

    }
}
}  // namespace Mycc::Codegen