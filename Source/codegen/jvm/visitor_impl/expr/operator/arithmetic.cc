//
// Created by chen_ on 2022/4/10.
//
#include "AST/expr/operator/arithmetic.h"

#include "codegen/jvm/JVMGenerator.h"

namespace Mycc::Codegen {
Status JVMGenerator::visit(std::unique_ptr<Mycc::AST::ArithmeticExpr> &p_expr) {
    p_expr->GetLHS()->visit(*this);
    p_expr->GetRHS()->visit(*this);
    GetOstream() << GetLineIndent() << "iadd\n";
}
}  // namespace Mycc::Codegen
