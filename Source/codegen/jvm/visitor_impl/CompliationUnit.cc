//
// Created by hanzech on 22-4-29.
//
#include "codegen/jvm/JVMGenerator.h"
#include "AST/CompilationUnit.h"
namespace Hzcc::Codegen {
Status JVMGenerator::VisitAllAST(
    const std::unique_ptr<Hzcc::AST::CompilationUnit> &p_expr) {
    // handle init section
    for (auto &decl : p_expr->GetDecls()){

    }
}
}  // namespace Hzcc::Codegen