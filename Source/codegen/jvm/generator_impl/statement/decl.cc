//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/decl.h"

#include "AST/type/Type.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"

namespace Mycc::Codegen {
void JVMGenerator::visit(Mycc::AST::DeclNode *p_expr) {
    auto var_name = p_expr->GetName();
    BindStack(var_name, TypeUtils::GetJVMTypename(p_expr->GetType()->GetName()),
              true);
}
void JVMGenerator::visit(Mycc::AST::VarDecl *p_expr) {}
}  // namespace Mycc::Codegen
